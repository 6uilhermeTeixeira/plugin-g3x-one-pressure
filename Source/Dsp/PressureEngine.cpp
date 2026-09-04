#include "PressureEngine.h"
#include <cmath>

namespace g3x
{
namespace
{
constexpr float meterFloor = -100.0f;
float coefficient(float ms, double sampleRate) noexcept
{
    return std::exp(-1.0f / (0.001f * juce::jmax(0.01f, ms) * static_cast<float>(sampleRate)));
}
float finite(float value) noexcept { return std::isfinite(value) ? value : 0.0f; }
}

void PressureEngine::prepare(double newSampleRate, int, int)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    inputGain.reset(sampleRate, 0.025);
    amountSmooth.reset(sampleRate, 0.025);
    rmsCoefficient = coefficient(20.0f, sampleRate);
    highPassCoefficient = std::exp(-juce::MathConstants<float>::twoPi * 80.0f / static_cast<float>(sampleRate));
    reset();
    setAmount(amount);
    setInputMode(inputMode);
}

void PressureEngine::reset() noexcept
{
    rmsState = envelopeDb = 0.0f;
    highPassState.fill(0.0f);
    previousInput.fill(0.0f);
    inputGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(
        inputMode == InputMode::pad ? -6.0f : inputMode == InputMode::drive ? 6.0f : 0.0f));
    amountSmooth.setCurrentAndTargetValue(amount);
}

void PressureEngine::setAmount(float newAmount) noexcept
{
    amount = juce::jlimit(0.0f, 1.0f, finite(newAmount));
    macro = mapAmount(amount);
    amountSmooth.setTargetValue(amount);
    updateCoefficients();
}

void PressureEngine::setInputMode(InputMode newMode) noexcept
{
    inputMode = newMode;
    const auto db = inputMode == InputMode::pad ? -6.0f : inputMode == InputMode::drive ? 6.0f : 0.0f;
    inputGain.setTargetValue(juce::Decibels::decibelsToGain(db));
}

MacroValues PressureEngine::mapAmount(float normalizedAmount) noexcept
{
    const auto x = juce::jlimit(0.0f, 1.0f, normalizedAmount);
    const auto energy = std::pow(x, 0.72f);
    MacroValues values;
    values.thresholdDb = juce::jmap(energy, 0.0f, 1.0f, -3.0f, -38.0f);
    values.ratio = 1.0f + 19.0f * std::pow(x, 1.35f);
    values.attackMs = juce::jmap(std::pow(x, 0.9f), 25.0f, 1.0f);
    values.releaseMs = juce::jmap(x, 250.0f, 45.0f);
    values.kneeDb = juce::jmap(x, 12.0f, 2.0f);
    values.wet = juce::jlimit(0.0f, 1.0f, 0.08f + 0.92f * std::pow(x, 0.65f));
    values.makeupDb = 8.0f * std::pow(x, 1.2f);
    values.saturation = juce::jlimit(0.0f, 1.0f, (x - 0.62f) / 0.38f);
    if (x <= 0.00001f)
    {
        values.thresholdDb = 0.0f;
        values.ratio = 1.0f;
        values.wet = values.makeupDb = values.saturation = 0.0f;
    }
    return values;
}

float PressureEngine::staticGainChangeDb(float inputDb, const MacroValues& values) noexcept
{
    if (!std::isfinite(inputDb))
        return 0.0f;
    const auto slope = 1.0f / juce::jmax(1.0f, values.ratio) - 1.0f;
    const auto distance = inputDb - values.thresholdDb;
    const auto halfKnee = juce::jmax(0.0f, values.kneeDb * 0.5f);
    if (distance <= -halfKnee)
        return 0.0f;
    if (distance >= halfKnee || halfKnee <= 0.0f)
        return slope * distance;
    const auto kneePosition = distance + halfKnee;
    return slope * kneePosition * kneePosition / (4.0f * halfKnee);
}

void PressureEngine::updateCoefficients() noexcept
{
    attackCoefficient = coefficient(macro.attackMs, sampleRate);
    releaseCoefficient = coefficient(macro.releaseMs, sampleRate);
}

float PressureEngine::detect(const juce::AudioBuffer<float>& buffer, int sample) noexcept
{
    float peak = 0.0f, energy = 0.0f;
    const auto channels = juce::jmin(2, buffer.getNumChannels());
    for (int channel = 0; channel < channels; ++channel)
    {
        const auto value = finite(buffer.getSample(channel, sample));
        const auto index = static_cast<size_t>(channel);
        const auto filtered = highPassCoefficient * (highPassState[index] + value - previousInput[index]);
        highPassState[index] = filtered;
        previousInput[index] = value;
        const auto magnitude = std::abs(filtered);
        peak = juce::jmax(peak, magnitude);
        energy += filtered * filtered;
    }
    energy /= static_cast<float>(juce::jmax(1, channels));
    rmsState = rmsCoefficient * rmsState + (1.0f - rmsCoefficient) * energy;
    const auto detector = 0.62f * peak + 0.38f * std::sqrt(juce::jmax(0.0f, rmsState));
    return juce::Decibels::gainToDecibels(detector, meterFloor);
}

float PressureEngine::shape(float sample, float driveAmount) const noexcept
{
    const auto drive = 1.0f + 2.5f * driveAmount;
    return std::tanh(sample * drive) / std::tanh(drive);
}

void PressureEngine::process(juce::AudioBuffer<float>& buffer) noexcept
{
    juce::ScopedNoDenormals noDenormals;
    const auto channels = juce::jmin(2, buffer.getNumChannels());
    std::array<float, 2> inputPeak {}, outputPeak {};

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto currentAmount = amountSmooth.getNextValue();
        const auto currentMacro = mapAmount(currentAmount);
        const auto trim = inputGain.getNextValue();

        for (int channel = 0; channel < channels; ++channel)
        {
            auto value = finite(buffer.getSample(channel, sample)) * trim;
            buffer.setSample(channel, sample, value);
            inputPeak[static_cast<size_t>(channel)] = juce::jmax(inputPeak[static_cast<size_t>(channel)], std::abs(value));
        }

        const auto targetDb = staticGainChangeDb(detect(buffer, sample), currentMacro);
        const auto envelopeCoefficient = targetDb < envelopeDb ? attackCoefficient
            : std::pow(releaseCoefficient, 1.0f + 1.8f * juce::jlimit(0.0f, 1.0f, std::abs(envelopeDb) / 18.0f));
        envelopeDb = envelopeCoefficient * envelopeDb + (1.0f - envelopeCoefficient) * targetDb;
        const auto compressionGain = juce::Decibels::decibelsToGain(envelopeDb);
        const auto makeup = juce::Decibels::decibelsToGain(currentMacro.makeupDb);

        for (int channel = 0; channel < channels; ++channel)
        {
            const auto dry = buffer.getSample(channel, sample);
            auto wet = dry * compressionGain;
            if (currentMacro.saturation > 0.0f)
                wet = shape(wet, currentMacro.saturation);
            auto output = juce::jmap(currentMacro.wet, dry, wet) * makeup;
            output = finite(output);
            buffer.setSample(channel, sample, output);
            outputPeak[static_cast<size_t>(channel)] = juce::jmax(outputPeak[static_cast<size_t>(channel)], std::abs(output));
        }
    }

    for (int channel = 0; channel < 2; ++channel)
    {
        meters.inputDb[static_cast<size_t>(channel)].store(
            juce::Decibels::gainToDecibels(inputPeak[static_cast<size_t>(channel)], meterFloor), std::memory_order_relaxed);
        meters.outputDb[static_cast<size_t>(channel)].store(
            juce::Decibels::gainToDecibels(outputPeak[static_cast<size_t>(channel)], meterFloor), std::memory_order_relaxed);
    }
    meters.gainReductionDb.store(juce::jmin(0.0f, envelopeDb), std::memory_order_relaxed);
}
}

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>

namespace g3x
{
enum class InputMode { pad = 0, normal, drive };

struct MeterSnapshot
{
    std::array<std::atomic<float>, 2> inputDb {{ -100.0f, -100.0f }};
    std::array<std::atomic<float>, 2> outputDb {{ -100.0f, -100.0f }};
    std::atomic<float> gainReductionDb { 0.0f };
};

struct MacroValues
{
    float thresholdDb = 0.0f;
    float ratio = 1.0f;
    float attackMs = 25.0f;
    float releaseMs = 250.0f;
    float kneeDb = 12.0f;
    float wet = 0.0f;
    float makeupDb = 0.0f;
    float saturation = 0.0f;
};

class PressureEngine
{
public:
    void prepare(double newSampleRate, int maximumBlockSize, int channels);
    void reset() noexcept;
    void setAmount(float newAmount) noexcept;
    void setInputMode(InputMode newMode) noexcept;
    void process(juce::AudioBuffer<float>& buffer) noexcept;

    [[nodiscard]] static MacroValues mapAmount(float normalizedAmount) noexcept;
    [[nodiscard]] static float staticGainChangeDb(float inputDb, const MacroValues&) noexcept;
    [[nodiscard]] const MeterSnapshot& getMeters() const noexcept { return meters; }

private:
    void updateCoefficients() noexcept;
    [[nodiscard]] float detect(const juce::AudioBuffer<float>&, int sample) noexcept;
    [[nodiscard]] float shape(float sample, float amount) const noexcept;

    double sampleRate = 44100.0;
    float amount = 0.0f;
    InputMode inputMode = InputMode::normal;
    MacroValues macro;
    float rmsState = 0.0f;
    std::array<float, 2> highPassState {};
    std::array<float, 2> previousInput {};
    float envelopeDb = 0.0f;
    float attackCoefficient = 0.0f;
    float releaseCoefficient = 0.0f;
    float rmsCoefficient = 0.0f;
    float highPassCoefficient = 0.0f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> inputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> amountSmooth;
    MeterSnapshot meters;
};
}

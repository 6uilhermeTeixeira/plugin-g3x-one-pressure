#include "Dsp/PressureEngine.h"
#include <cmath>
#include <iostream>
#include <limits>

namespace
{
int failures = 0;
void expect(bool condition, const char* message)
{
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; ++failures; }
}
void expectNear(float actual, float expected, float tolerance, const char* message)
{
    expect(std::abs(actual - expected) <= tolerance, message);
}
}

int main()
{
    const auto neutral = g3x::PressureEngine::mapAmount(0.0f);
    expectNear(neutral.ratio, 1.0f, 0.0001f, "zero amount ratio is neutral");
    expectNear(neutral.wet, 0.0f, 0.0001f, "zero amount has no wet signal");
    expectNear(neutral.makeupDb, 0.0f, 0.0001f, "zero amount has no makeup");

    const auto low = g3x::PressureEngine::mapAmount(0.25f);
    const auto middle = g3x::PressureEngine::mapAmount(0.5f);
    const auto high = g3x::PressureEngine::mapAmount(1.0f);
    expect(low.ratio < middle.ratio && middle.ratio < high.ratio, "ratio rises monotonically");
    expect(low.attackMs > middle.attackMs && middle.attackMs > high.attackMs, "attack becomes faster");
    expect(low.releaseMs > middle.releaseMs && middle.releaseMs > high.releaseMs, "release becomes faster");
    expectNear(high.attackMs, 1.0f, 0.01f, "maximum attack mapping");
    expectNear(high.releaseMs, 45.0f, 0.01f, "maximum release mapping");
    expectNear(high.saturation, 1.0f, 0.001f, "maximum saturation mapping");
    expect(g3x::PressureEngine::staticGainChangeDb(0.0f, high) < -30.0f,
           "maximum pressure creates aggressive reduction");

    g3x::PressureEngine engine;
    engine.prepare(48000.0, 512, 2);
    engine.setAmount(0.0f);
    engine.setInputMode(g3x::InputMode::normal);
    juce::AudioBuffer<float> audio(2, 512);
    for (int channel = 0; channel < 2; ++channel)
        for (int sample = 0; sample < audio.getNumSamples(); ++sample)
            audio.setSample(channel, sample, 0.3f * std::sin(0.037f * static_cast<float>(sample + channel)));
    auto original = audio;
    engine.process(audio);
    for (int channel = 0; channel < 2; ++channel)
        for (int sample = 0; sample < audio.getNumSamples(); ++sample)
            expectNear(audio.getSample(channel, sample), original.getSample(channel, sample), 1.0e-6f,
                       "amount zero null test");

    audio.clear();
    audio.setSample(0, 0, std::numeric_limits<float>::quiet_NaN());
    audio.setSample(1, 0, std::numeric_limits<float>::infinity());
    engine.process(audio);
    for (int channel = 0; channel < 2; ++channel)
        for (int sample = 0; sample < audio.getNumSamples(); ++sample)
            expect(std::isfinite(audio.getSample(channel, sample)), "output remains finite");

    engine.reset();
    engine.setAmount(1.0f);
    for (int block = 0; block < 12; ++block)
    {
        for (int channel = 0; channel < 2; ++channel)
            for (int sample = 0; sample < audio.getNumSamples(); ++sample)
                audio.setSample(channel, sample, 0.5f * std::sin(
                    juce::MathConstants<float>::twoPi * 1000.0f * static_cast<float>(block * 512 + sample) / 48000.0f));
        engine.process(audio);
    }
    expect(engine.getMeters().gainReductionDb.load() < -6.0f,
           "sustained material keeps the compressor engaged after the sidechain HPF");

    if (failures == 0) std::cout << "All G3X Pressure DSP tests passed\n";
    return failures == 0 ? 0 : 1;
}

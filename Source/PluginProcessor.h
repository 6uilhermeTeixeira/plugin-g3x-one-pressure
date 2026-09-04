#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Dsp/PressureEngine.h"

class G3XPressureAudioProcessor final : public juce::AudioProcessor
{
public:
    using AudioProcessor::processBlock;
    G3XPressureAudioProcessor();
    ~G3XPressureAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 6; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    const g3x::MeterSnapshot& getMeters() const noexcept { return engine.getMeters(); }
    juce::AudioProcessorValueTreeState state;

private:
    g3x::PressureEngine engine;
    std::atomic<float>* amount = nullptr;
    std::atomic<float>* inputMode = nullptr;
    int currentProgram = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(G3XPressureAudioProcessor)
};

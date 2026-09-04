#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/Meter.h"

class G3XOnePressureAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit G3XOnePressureAudioProcessorEditor(G3XOnePressureAudioProcessor&);
    ~G3XOnePressureAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    G3XOnePressureAudioProcessor& processor;
    g3x::ui::LookAndFeel lookAndFeel;
    juce::Slider pressure;
    juce::ComboBox inputMode;
    juce::Label inputLabel, reductionLabel, inputLedLabel, outputLedLabel;
    g3x::ui::Meter meter;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> inputAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(G3XOnePressureAudioProcessorEditor)
};

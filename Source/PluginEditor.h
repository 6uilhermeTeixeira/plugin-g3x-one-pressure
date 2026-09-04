#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/Meter.h"

class G3XPressureAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit G3XPressureAudioProcessorEditor(G3XPressureAudioProcessor&);
    ~G3XPressureAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    G3XPressureAudioProcessor& processor;
    g3x::ui::LookAndFeel lookAndFeel;
    juce::Slider pressure;
    juce::ComboBox inputMode;
    juce::Label inputLabel, reductionLabel, inputLedLabel, outputLedLabel;
    g3x::ui::Meter meter;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> inputAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(G3XPressureAudioProcessorEditor)
};

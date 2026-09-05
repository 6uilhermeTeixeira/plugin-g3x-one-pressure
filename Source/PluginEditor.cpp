#include "PluginEditor.h"

G3XOnePressureAudioProcessorEditor::G3XOnePressureAudioProcessorEditor(G3XOnePressureAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lookAndFeel);
    setResizable(true, true);
    setResizeLimits(420, 454, 760, 821);
    getConstrainer()->setFixedAspectRatio(500.0 / 540.0);
    setSize(500, 540);

    pressure.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pressure.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    pressure.setNumDecimalPlacesToDisplay(1);
    pressure.setDoubleClickReturnValue(true, 0.0);
    pressure.setTitle("Pressure");
    pressure.setDescription("Main character compression amount from zero to ten");
    addAndMakeVisible(pressure);

    inputMode.addItemList({ "Pad", "Normal", "Drive" }, 1);
    inputMode.setTitle("Input");
    inputMode.setDescription("Input level: Pad minus six dB, Normal, or Drive plus six dB");
    addAndMakeVisible(inputMode);

    for (int index = 0; index < processor.getNumPrograms(); ++index)
        presetBox.addItem(processor.getProgramName(index), index + 1);
    presetBox.setSelectedItemIndex(processor.getCurrentProgram(), juce::dontSendNotification);
    presetBox.setTitle("Factory preset");
    presetBox.setDescription("Loads a G3X One Pressure starting point");
    presetBox.onChange = [this] { processor.setCurrentProgram(presetBox.getSelectedItemIndex()); };
    bypassButton.setTitle("Plugin bypass");
    bypassButton.setDescription("Bypasses pressure processing");
    statusLabel.setText("CHARACTER COMPRESSION", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centredRight);
    statusLabel.setColour(juce::Label::textColourId, juce::Colour(g3x::ui::Colours::muted));
    statusLabel.setFont(juce::FontOptions { 10.0f }.withStyle("Bold"));
    for (auto* component : std::initializer_list<juce::Component*> {
             &presetBox, &bypassButton, &statusLabel })
        addAndMakeVisible(component);

    inputLabel.setText("INPUT", juce::dontSendNotification);
    reductionLabel.setText("GAIN REDUCTION", juce::dontSendNotification);
    inputLedLabel.setText("IN", juce::dontSendNotification);
    outputLedLabel.setText("OUT", juce::dontSendNotification);
    for (auto* label : { &inputLabel, &reductionLabel, &inputLedLabel, &outputLedLabel })
    {
        addAndMakeVisible(label);
        label->setJustificationType(juce::Justification::centred);
        label->setColour(juce::Label::textColourId, juce::Colour(g3x::ui::Colours::muted));
        label->setFont(juce::FontOptions { 11.0f }.withStyle("Bold"));
    }
    addAndMakeVisible(meter);
    meter.setTitle("Dynamics meter");
    meter.setDescription("Input peak, gain reduction and output peak");

    amountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.state, "amount", pressure);
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        p.state, "inputMode", inputMode);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        p.state, "bypass", bypassButton);
    startTimerHz(45);
}

G3XOnePressureAudioProcessorEditor::~G3XOnePressureAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void G3XOnePressureAudioProcessorEditor::paint(juce::Graphics& g)
{
    using C = g3x::ui::Colours;
    auto area = getLocalBounds().toFloat();
    g.setGradientFill({ juce::Colour(C::panel).brighter(0.06f), 0.0f, 0.0f,
                        juce::Colour(C::background), 0.0f, area.getBottom(), false });
    g.fillAll();
    auto header = getLocalBounds().removeFromTop(78);
    g.setColour(juce::Colour(C::cyan));
    g.fillRect(header.removeFromBottom(2));
    g.setColour(juce::Colour(C::ink));
    g.setFont(juce::FontOptions { 25.0f }.withStyle("Bold"));
    g.drawText("G3X", header.reduced(22, 8).removeFromLeft(68), juce::Justification::centredLeft);
    g.setColour(juce::Colour(C::violet));
    g.setFont(juce::FontOptions { 17.0f });
    g.drawFittedText("ONE PRESSURE", { 88, 8, 96, 60 }, juce::Justification::centredLeft,
                     1, 0.65f);
    const auto panel = juce::Rectangle<float>(20.0f, 92.0f, area.getWidth() - 40.0f,
                                               area.getHeight() - 126.0f);
    g.setColour(juce::Colour(C::panel));
    g.fillRoundedRectangle(panel, 18.0f);
    g.setColour(juce::Colour(C::cyan).withAlpha(0.18f));
    g.drawRoundedRectangle(panel, 18.0f, 1.0f);

    g.setColour(juce::Colour(C::muted));
    g.setFont(juce::FontOptions { 9.0f });
    g.drawText("WEIGHT  /  PUNCH  /  GLUE", getLocalBounds().removeFromBottom(24),
               juce::Justification::centred);
}

void G3XOnePressureAudioProcessorEditor::resized()
{
    auto header = getLocalBounds().removeFromTop(78).reduced(18, 17);
    bypassButton.setBounds(header.removeFromRight(80));
    header.removeFromRight(8);
    presetBox.setBounds(header.removeFromRight(210));
    auto body = getLocalBounds().withTrimmedTop(104).withTrimmedBottom(46).reduced(32, 0);
    auto meterArea = body.removeFromBottom(58);
    auto meterLabels = meterArea.removeFromTop(18);
    inputLedLabel.setBounds(meterLabels.removeFromLeft(44));
    outputLedLabel.setBounds(meterLabels.removeFromRight(44));
    reductionLabel.setBounds(meterLabels);
    meter.setBounds(meterArea.reduced(3, 2));
    auto accessory = body.removeFromBottom(44);
    inputLabel.setBounds(accessory.removeFromLeft(54));
    inputMode.setBounds(accessory.removeFromLeft(132).reduced(3, 7));
    statusLabel.setBounds(accessory.reduced(8, 7));
    pressure.setBounds(body.reduced(62, 0));
}

void G3XOnePressureAudioProcessorEditor::timerCallback()
{
    const auto& values = processor.getMeters();
    meter.setLevel(values.inputDb[0].load(std::memory_order_relaxed),
                   values.inputDb[1].load(std::memory_order_relaxed),
                   values.outputDb[0].load(std::memory_order_relaxed),
                   values.outputDb[1].load(std::memory_order_relaxed),
                   values.gainReductionDb.load(std::memory_order_relaxed));
}

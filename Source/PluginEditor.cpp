#include "PluginEditor.h"

G3XPressureAudioProcessorEditor::G3XPressureAudioProcessorEditor(G3XPressureAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lookAndFeel);
    setResizable(true, true);
    setResizeLimits(360, 460, 720, 920);
    getConstrainer()->setFixedAspectRatio(0.7826);
    setSize(450, 575);

    pressure.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pressure.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 104, 38);
    pressure.setNumDecimalPlacesToDisplay(1);
    pressure.setDoubleClickReturnValue(true, 0.0);
    pressure.setTitle("Pressure");
    pressure.setDescription("Main character compression amount from zero to ten");
    addAndMakeVisible(pressure);

    inputMode.addItemList({ "Pad", "Normal", "Drive" }, 1);
    inputMode.setTitle("Input");
    inputMode.setDescription("Input level: Pad minus six dB, Normal, or Drive plus six dB");
    addAndMakeVisible(inputMode);

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
    startTimerHz(45);
}

G3XPressureAudioProcessorEditor::~G3XPressureAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void G3XPressureAudioProcessorEditor::paint(juce::Graphics& g)
{
    using C = g3x::ui::Colours;
    g.fillAll(juce::Colour(C::background));
    auto area = getLocalBounds().toFloat();
    juce::ColourGradient glow(juce::Colour(C::violet).withAlpha(0.14f), area.getCentreX(), area.getCentreY(),
                              juce::Colours::transparentBlack, 0.0f, area.getBottom(), true);
    g.setGradientFill(glow);
    g.fillRect(area);

    g.setColour(juce::Colour(C::ink));
    g.setFont(juce::FontOptions { 22.0f }.withStyle("Bold"));
    g.drawText("G3X", 24, 20, 64, 28, juce::Justification::centredLeft);
    g.setColour(juce::Colour(C::cyan));
    g.drawText("PRESSURE", 83, 20, 150, 28, juce::Justification::centredLeft);

    const auto panel = juce::Rectangle<float>(22.0f, 68.0f, area.getWidth() - 44.0f, area.getHeight() - 90.0f);
    g.setColour(juce::Colour(C::panel));
    g.fillRoundedRectangle(panel, 18.0f);
    g.setColour(juce::Colour(C::cyan).withAlpha(0.18f));
    g.drawRoundedRectangle(panel, 18.0f, 1.0f);

    auto dial = pressure.getBounds().toFloat().reduced(21.0f, 42.0f);
    g.setFont(juce::FontOptions { 10.0f }.withStyle("Bold"));
    g.setColour(juce::Colour(C::muted));
    for (int i = 0; i <= 10; ++i)
    {
        const auto angle = juce::jmap(static_cast<float>(i), 0.0f, 10.0f,
                                     juce::MathConstants<float>::pi * 1.2f,
                                     juce::MathConstants<float>::pi * 2.8f);
        const auto point = dial.getCentre().getPointOnCircumference(dial.getWidth() * 0.52f, angle);
        g.drawText(juce::String(i), static_cast<int>(point.x - 9.0f), static_cast<int>(point.y - 7.0f),
                   18, 14, juce::Justification::centred);
    }
}

void G3XPressureAudioProcessorEditor::resized()
{
    auto panel = getLocalBounds().reduced(30);
    panel.removeFromTop(55);
    auto input = panel.removeFromBottom(62);
    inputLabel.setBounds(input.removeFromLeft(64));
    inputMode.setBounds(input.reduced(4, 11));
    auto meterArea = panel.removeFromBottom(66);
    auto meterLabels = meterArea.removeFromTop(18);
    inputLedLabel.setBounds(meterLabels.removeFromLeft(44));
    outputLedLabel.setBounds(meterLabels.removeFromRight(44));
    reductionLabel.setBounds(meterLabels);
    meter.setBounds(meterArea.reduced(3, 2));
    pressure.setBounds(panel.reduced(9, 0));
}

void G3XPressureAudioProcessorEditor::timerCallback()
{
    const auto& values = processor.getMeters();
    meter.setLevel(values.inputDb[0].load(std::memory_order_relaxed),
                   values.inputDb[1].load(std::memory_order_relaxed),
                   values.outputDb[0].load(std::memory_order_relaxed),
                   values.outputDb[1].load(std::memory_order_relaxed),
                   values.gainReductionDb.load(std::memory_order_relaxed));
}

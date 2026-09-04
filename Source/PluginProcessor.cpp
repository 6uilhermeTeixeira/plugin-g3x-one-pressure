#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace IDs
{
constexpr auto amount = "amount";
constexpr auto inputMode = "inputMode";
}

G3XOnePressureAudioProcessor::G3XOnePressureAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      state(*this, nullptr, "G3XPressureState", createParameterLayout())
{
    amount = state.getRawParameterValue(IDs::amount);
    inputMode = state.getRawParameterValue(IDs::inputMode);
}

juce::AudioProcessorValueTreeState::ParameterLayout G3XOnePressureAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { IDs::amount, 1 }, "Pressure",
        juce::NormalisableRange<float> { 0.0f, 10.0f, 0.01f }, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction([] (float value, int)
        { return juce::String(value, 1); })));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { IDs::inputMode, 1 }, "Input",
        juce::StringArray { "Pad", "Normal", "Drive" }, 1));
    return layout;
}

void G3XOnePressureAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    engine.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

bool G3XOnePressureAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void G3XOnePressureAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    for (int channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel)
        buffer.clear(channel, 0, buffer.getNumSamples());
    engine.setAmount(amount->load() * 0.1f);
    engine.setInputMode(static_cast<g3x::InputMode>(juce::roundToInt(inputMode->load())));
    engine.process(buffer);
}

void G3XOnePressureAudioProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    auto tree = state.copyState();
    tree.setProperty("stateVersion", 1, nullptr);
    if (auto xml = tree.createXml())
        copyXmlToBinary(*xml, destination);
}

void G3XOnePressureAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(state.state.getType()))
            state.replaceState(juce::ValueTree::fromXml(*xml));
}

const juce::String G3XOnePressureAudioProcessor::getProgramName(int index)
{
    static constexpr std::array names { "Neutral", "Drum Glue", "Parallel Punch",
                                        "Room Pump", "Loop Smash", "Aggressive Bus" };
    return juce::isPositiveAndBelow(index, static_cast<int>(names.size())) ? names[static_cast<size_t>(index)] : "";
}

void G3XOnePressureAudioProcessor::setCurrentProgram(int index)
{
    static constexpr std::array amounts { 0.0f, 2.8f, 4.6f, 6.5f, 8.2f, 10.0f };
    static constexpr std::array modes { 1.0f, 1.0f, 1.0f, 2.0f, 0.0f, 0.0f };
    if (!juce::isPositiveAndBelow(index, static_cast<int>(amounts.size())))
        return;
    currentProgram = index;
    auto set = [this](const char* id, float value)
    {
        if (auto* parameter = state.getParameter(id))
            parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    };
    set(IDs::amount, amounts[static_cast<size_t>(index)]);
    set(IDs::inputMode, modes[static_cast<size_t>(index)]);
}

juce::AudioProcessorEditor* G3XOnePressureAudioProcessor::createEditor()
{
    return new G3XOnePressureAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new G3XOnePressureAudioProcessor();
}

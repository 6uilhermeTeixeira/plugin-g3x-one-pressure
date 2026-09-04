#include "Meter.h"
#include "LookAndFeel.h"

namespace g3x::ui
{
void Meter::setLevel(float inputLeft, float inputRight, float outputLeft, float outputRight, float reduction)
{
    input = juce::jmax(inputLeft, inputRight);
    output = juce::jmax(outputLeft, outputRight);
    gainReduction = reduction;
    repaint();
}

void Meter::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    auto drawLed = [&g](juce::Rectangle<float> bounds, float db, juce::Colour colour)
    {
        const auto active = db > -0.5f;
        g.setColour(active ? colour : juce::Colour(Colours::muted).withAlpha(0.16f));
        g.fillEllipse(bounds);
        if (active)
        {
            g.setColour(colour.withAlpha(0.28f));
            g.drawEllipse(bounds.expanded(3.0f), 3.0f);
        }
    };
    auto left = area.removeFromLeft(34.0f);
    auto right = area.removeFromRight(34.0f);
    drawLed(left.withSizeKeepingCentre(11.0f, 11.0f), input, juce::Colour(Colours::amber));
    drawLed(right.withSizeKeepingCentre(11.0f, 11.0f), output, juce::Colour(Colours::red));

    auto bar = area.reduced(7.0f, 13.0f);
    g.setColour(juce::Colour(Colours::background));
    g.fillRoundedRectangle(bar, 4.0f);
    const auto amount = juce::jlimit(0.0f, 1.0f, -gainReduction / 24.0f);
    auto active = bar.withWidth(bar.getWidth() * amount);
    juce::ColourGradient gradient(juce::Colour(Colours::cyan), bar.getX(), 0.0f,
                                  juce::Colour(Colours::violet), bar.getRight(), 0.0f, false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(active, 4.0f);
}
}

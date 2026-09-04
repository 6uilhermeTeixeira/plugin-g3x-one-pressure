#include "LookAndFeel.h"

namespace g3x::ui
{
LookAndFeel::LookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(Colours::ink));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(Colours::background));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(Colours::background));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(Colours::cyan).withAlpha(0.5f));
    setColour(juce::ComboBox::textColourId, juce::Colour(Colours::ink));
}

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                   float position, float start, float end, juce::Slider&)
{
    auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                         static_cast<float>(width), static_cast<float>(height)).reduced(13.0f);
    const auto diameter = juce::jmin(bounds.getWidth(), bounds.getHeight());
    bounds = bounds.withSizeKeepingCentre(diameter, diameter);
    const auto centre = bounds.getCentre();
    const auto radius = diameter * 0.5f;
    const auto angle = juce::jmap(position, start, end);

    g.setColour(juce::Colour(Colours::background));
    g.fillEllipse(bounds);
    g.setColour(juce::Colour(Colours::muted).withAlpha(0.25f));
    g.drawEllipse(bounds, 3.0f);

    juce::Path base, active;
    base.addCentredArc(centre.x, centre.y, radius - 5.0f, radius - 5.0f, 0.0f, start, end, true);
    active.addCentredArc(centre.x, centre.y, radius - 5.0f, radius - 5.0f, 0.0f, start, angle, true);
    g.setColour(juce::Colour(Colours::muted).withAlpha(0.25f));
    g.strokePath(base, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    juce::ColourGradient gradient(juce::Colour(Colours::cyan), bounds.getX(), centre.y,
                                  juce::Colour(Colours::violet), bounds.getRight(), centre.y, false);
    g.setGradientFill(gradient);
    g.strokePath(active, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    pointer.addRoundedRectangle(-3.0f, -radius + 22.0f, 6.0f, radius * 0.3f, 3.0f);
    g.setColour(juce::Colour(Colours::amber));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));

    g.setColour(juce::Colour(Colours::panel));
    g.fillEllipse(bounds.reduced(radius * 0.28f));
    g.setColour(juce::Colour(Colours::ink).withAlpha(0.12f));
    for (int i = 0; i < 10; ++i)
    {
        const auto tickAngle = juce::jmap(static_cast<float>(i), 0.0f, 9.0f, start, end);
        const auto a = centre.getPointOnCircumference(radius * 0.58f, tickAngle);
        const auto b = centre.getPointOnCircumference(radius * 0.7f, tickAngle);
        g.drawLine({ a, b }, 2.0f);
    }
}
}

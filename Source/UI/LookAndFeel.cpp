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
    setColour(juce::ComboBox::arrowColourId, juce::Colour(Colours::cyan));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(Colours::panel));
    setColour(juce::PopupMenu::textColourId, juce::Colour(Colours::ink));
    setColour(juce::PopupMenu::highlightedBackgroundColourId,
              juce::Colour(Colours::violet).darker(0.35f));
}

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                   float position, float start, float end, juce::Slider& slider)
{
    const auto radius = 0.5f * static_cast<float>(std::min(width, height)) - 13.0f;
    const juce::Point<float> centre { static_cast<float>(x + width / 2),
                                      static_cast<float>(y + height / 2) };
    const auto circle = juce::Rectangle<float>(radius * 2.0f, radius * 2.0f).withCentre(centre);
    g.setColour(juce::Colour(Colours::background));
    g.fillEllipse(circle);
    g.setColour(juce::Colour(Colours::muted).withAlpha(0.45f));
    g.drawEllipse(circle, 2.0f);
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centre.x, centre.y, radius + 7.0f, radius + 7.0f,
                                0.0f, start, end, true);
    g.setColour(juce::Colour(Colours::muted).withAlpha(0.35f));
    g.strokePath(backgroundArc, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));
    const auto angle = start + position * (end - start);
    juce::Path activeArc;
    activeArc.addCentredArc(centre.x, centre.y, radius + 7.0f, radius + 7.0f,
                            0.0f, start, angle, true);
    g.setGradientFill({ juce::Colour(Colours::cyan), circle.getX(), circle.getBottom(),
                        juce::Colour(Colours::violet), circle.getRight(), circle.getY(), false });
    g.strokePath(activeArc, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved,
                                                 juce::PathStrokeType::rounded));
    juce::Path pointer;
    pointer.addRoundedRectangle(-3.0f, -radius + 19.0f, 6.0f, radius * 0.48f, 3.0f);
    g.setColour(juce::Colour(Colours::amber));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
    if (slider.hasKeyboardFocus(false))
    {
        g.setColour(juce::Colour(Colours::ink));
        g.drawRoundedRectangle(circle.expanded(16.0f), 20.0f, 1.5f);
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                   bool highlighted, bool)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    g.setColour(button.getToggleState() ? juce::Colour(Colours::red).darker(0.25f)
                                        : juce::Colour(Colours::background));
    g.fillRoundedRectangle(bounds, 7.0f);
    g.setColour(button.getToggleState() ? juce::Colour(Colours::red)
                                        : juce::Colour(Colours::cyan).withAlpha(highlighted ? 0.8f : 0.4f));
    g.drawRoundedRectangle(bounds, 7.0f, 1.5f);
    g.setColour(juce::Colour(Colours::ink));
    g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred);
}
}

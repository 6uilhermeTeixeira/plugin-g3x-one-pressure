#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace g3x::ui
{
struct Colours
{
    static constexpr juce::uint32 background = 0xff0d1217;
    static constexpr juce::uint32 panel = 0xff151d25;
    static constexpr juce::uint32 ink = 0xffedf2ef;
    static constexpr juce::uint32 muted = 0xff84939d;
    static constexpr juce::uint32 cyan = 0xff53d8e6;
    static constexpr juce::uint32 violet = 0xff9a7cff;
    static constexpr juce::uint32 amber = 0xffffb44b;
    static constexpr juce::uint32 red = 0xffff5b62;
};

class LookAndFeel final : public juce::LookAndFeel_V4
{
public:
    LookAndFeel();
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
};
}

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace g3x::ui
{
class Meter final : public juce::Component
{
public:
    void setLevel(float inputLeft, float inputRight, float outputLeft, float outputRight, float reduction);
    void paint(juce::Graphics&) override;
private:
    float input = -100.0f, output = -100.0f, gainReduction = 0.0f;
};
}

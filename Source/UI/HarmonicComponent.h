/*
  ==============================================================================

   Harmonic component

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomComponent.h"

//==============================================================================
/*
*/
class HarmonicComponent : public CustomComponent
{
public:
    HarmonicComponent (juce::AudioProcessorValueTreeState& apvts, juce::String oscId, juce::String gainId);
    ~HarmonicComponent() override;

    void resized() override;

private:
    SliderWithLabel gain;

    static constexpr int sliderWidth = 70;
    static constexpr int sliderHeight = 70;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicComponent)
};

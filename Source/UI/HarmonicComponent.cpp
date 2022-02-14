/*
  ==============================================================================

    HarmonicComponent.cpp
    Created: 11 Feb 2022 1:11:16pm
    Author:  morchella

  ==============================================================================
*/


#include <JuceHeader.h>
#include "HarmonicComponent.h"


HarmonicComponent::HarmonicComponent (juce::AudioProcessorValueTreeState& apvts, juce::String oscId, juce::String gainId)
 : gain ("Gain", gainId, apvts, sliderWidth, sliderHeight, juce::Slider::SliderStyle::LinearVertical)
{
    addAndMakeVisible (gain);
}

HarmonicComponent::~HarmonicComponent()
{
}


void HarmonicComponent::resized()
{
    const auto startX = 18;
    const auto startY = 80;
    const auto width = 70;
    const auto height = 100;

    gain.setBounds (18, 30, width, height);

}

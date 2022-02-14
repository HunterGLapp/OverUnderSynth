/*
  ==============================================================================

    SynthVoice.h
    Created: 10 Dec 2020 1:55:41pm
    Author:  Joshua Hodge

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "Data/OscData.h"
#include "Data/FilterData.h"
#include "Data/AdsrData.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void pitchWheelMoved (int newPitchWheelValue) override;
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    void renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;

    void reset();

    std::array<OscData, 2>& getOscillator1() { return osc1; }
    std::array<OscData, 2>& getSub1() { return sub1; }
    std::array<OscData, 2>& getSub2() { return sub2; }
    std::array<OscData, 2>& getSub3() { return sub3; }
    std::array<OscData, 2>& getSub4() { return sub4; }
    std::array<OscData, 2>& getSub5() { return sub5; }
    std::array<OscData, 2>& getSub6() { return sub6; }
    std::array<OscData, 2>& getSub7() { return sub7; }
    std::array<OscData, 2>& getSub8() { return sub8; }
    std::array<OscData, 2>& getSub9() { return sub9; }
    std::array<OscData, 2>& getOv1()  { return ov1; }
    std::array<OscData, 2>& getOv2()  { return ov2; }
    std::array<OscData, 2>& getOv3()  { return ov3; }
    std::array<OscData, 2>& getOv4()  { return ov4; }
    std::array<OscData, 2>& getOv5()  { return ov5; }
    std::array<OscData, 2>& getOv6()  { return ov6; }
    std::array<OscData, 2>& getOv7()  { return ov7; }
    std::array<OscData, 2>& getOv8()  { return ov8; }
    std::array<OscData, 2>& getOv9()  { return ov9; }



    AdsrData& getAdsr() { return adsr; }
    AdsrData& getFilterAdsr() { return filterAdsr; }
    float getFilterAdsrOutput() { return filterAdsrOutput; }
    void updateModParams (const int filterType, const float filterCutoff, const float filterResonance, const float adsrDepth, const float lfoFreq, const float lfoDepth);

private:
    static constexpr int numChannelsToProcess { 2 };
    std::array<OscData, numChannelsToProcess> osc1;
    std::array<OscData, numChannelsToProcess> sub1;
    std::array<OscData, numChannelsToProcess> sub2;
    std::array<OscData, numChannelsToProcess> sub3;
    std::array<OscData, numChannelsToProcess> sub4;
    std::array<OscData, numChannelsToProcess> sub5;
    std::array<OscData, numChannelsToProcess> sub6;
    std::array<OscData, numChannelsToProcess> sub7;
    std::array<OscData, numChannelsToProcess> sub8;
    std::array<OscData, numChannelsToProcess> sub9;
    std::array<OscData, numChannelsToProcess> ov1;
    std::array<OscData, numChannelsToProcess> ov2;
    std::array<OscData, numChannelsToProcess> ov3;
    std::array<OscData, numChannelsToProcess> ov4;
    std::array<OscData, numChannelsToProcess> ov5;
    std::array<OscData, numChannelsToProcess> ov6;
    std::array<OscData, numChannelsToProcess> ov7;
    std::array<OscData, numChannelsToProcess> ov8;
    std::array<OscData, numChannelsToProcess> ov9;


    std::array<FilterData, numChannelsToProcess> filter;
    std::array<juce::dsp::Oscillator<float>, numChannelsToProcess> lfo;
    AdsrData adsr;
    AdsrData filterAdsr;
    juce::AudioBuffer<float> synthBuffer;
    float filterAdsrOutput { 0.0f };
    std::array<float, numChannelsToProcess> lfoOutput { 0.0f, 0.0f };


    juce::dsp::Gain<float> gain;
    bool isPrepared { false };
};

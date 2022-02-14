/*
  ==============================================================================

    SynthVoice.cpp
    Created: 10 Dec 2020 1:55:41pm
    Author:  Joshua Hodge

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    for (int i = 0; i < 2; i++)
    {
        osc1[i].setFreq (midiNoteNumber);
        sub1[i].setSubFreq(midiNoteNumber, 2);
        sub2[i].setSubFreq(midiNoteNumber, 3);
        sub3[i].setSubFreq(midiNoteNumber, 4);
        sub4[i].setSubFreq(midiNoteNumber, 5);
        sub5[i].setSubFreq(midiNoteNumber, 6);
        sub6[i].setSubFreq(midiNoteNumber, 7);
        sub7[i].setSubFreq(midiNoteNumber, 8);
        sub8[i].setSubFreq(midiNoteNumber, 9);
        sub9[i].setSubFreq(midiNoteNumber, 10);
        ov1[i].setHarmonicFreq(midiNoteNumber, 2);
        ov2[i].setHarmonicFreq(midiNoteNumber, 3);
        ov3[i].setHarmonicFreq(midiNoteNumber, 4);
        ov4[i].setHarmonicFreq(midiNoteNumber, 5);
        ov5[i].setHarmonicFreq(midiNoteNumber, 6);
        ov6[i].setHarmonicFreq(midiNoteNumber, 7);
        ov7[i].setHarmonicFreq(midiNoteNumber, 8);
        ov8[i].setHarmonicFreq(midiNoteNumber, 9);
        ov9[i].setHarmonicFreq(midiNoteNumber, 10);

        //osc2[i].setFreq (midiNoteNumber);
    }

    adsr.noteOn();
    filterAdsr.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    adsr.noteOff();
    filterAdsr.noteOff();

    if (! allowTailOff || ! adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{

}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{

}

void SynthVoice::prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels)
{
    reset();

    adsr.setSampleRate (sampleRate);
    filterAdsr.setSampleRate (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    for (int ch = 0; ch < numChannelsToProcess; ch++)
    {
        osc1[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub1[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub2[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub3[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub4[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub5[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub6[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub7[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub8[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        sub9[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov1[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov2[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov3[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov4[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov5[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov6[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov7[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov8[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        ov9[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);

        //osc2[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        filter[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        lfo[ch].prepare (spec);
        lfo[ch].initialise ([](float x) { return std::sin (x); });
    }

    gain.prepare (spec);
    gain.setGainLinear (0.07f);

    isPrepared = true;
}

void SynthVoice::renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples)
{
    jassert (isPrepared);

    if (! isVoiceActive())
        return;

    synthBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);

    filterAdsr.applyEnvelopeToBuffer (synthBuffer, 0, synthBuffer.getNumSamples());
    filterAdsrOutput = filterAdsr.getNextSample();

    synthBuffer.clear();

    for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
    {
        auto* buffer = synthBuffer.getWritePointer (ch, 0);

        for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
        {
            buffer[s] = osc1[ch].processNextSample (buffer[s]) +
                        sub1[ch].processNextSample (buffer[s]) +
                        sub2[ch].processNextSample (buffer[s]) +
                        sub3[ch].processNextSample (buffer[s]) +
                        sub4[ch].processNextSample (buffer[s]) +
                        sub5[ch].processNextSample (buffer[s]) +
                        sub6[ch].processNextSample (buffer[s]) +
                        sub7[ch].processNextSample (buffer[s]) +
                        sub8[ch].processNextSample (buffer[s]) +
                        sub9[ch].processNextSample (buffer[s]) +
                        ov1[ch].processNextSample  (buffer[s]) +
                        ov2[ch].processNextSample  (buffer[s]) +
                        ov3[ch].processNextSample  (buffer[s]) +
                        ov4[ch].processNextSample  (buffer[s]) +
                        ov5[ch].processNextSample  (buffer[s]) +
                        ov6[ch].processNextSample  (buffer[s]) +
                        ov7[ch].processNextSample  (buffer[s]) +
                        ov8[ch].processNextSample  (buffer[s]) +
                        ov9[ch].processNextSample  (buffer[s]);

        }
    }

    juce::dsp::AudioBlock<float> audioBlock { synthBuffer };
    gain.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
    adsr.applyEnvelopeToBuffer (synthBuffer, 0, synthBuffer.getNumSamples());

    for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
    {
        auto* buffer = synthBuffer.getWritePointer (ch, 0);

        for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
        {
            //lfoOutput[ch] = lfo[ch].processSample (synthBuffer.getSample (ch, s));
            buffer[s] = filter[ch].processNextSample (ch, synthBuffer.getSample (ch, s));
        }
    }

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom (channel, startSample, synthBuffer, channel, 0, numSamples);

        if (! adsr.isActive())
            clearCurrentNote();
    }
}

void SynthVoice::reset()
{
    gain.reset();
    adsr.reset();
    filterAdsr.reset();
}

void SynthVoice::updateModParams (const int filterType, const float filterCutoff, const float filterResonance, const float adsrDepth, const float lfoFreq, const float lfoDepth)
{
    auto cutoff = (adsrDepth * filterAdsrOutput) + filterCutoff;
    cutoff = std::clamp<float> (cutoff, 20.0f, 20000.0f);

    for (int ch = 0; ch < numChannelsToProcess; ++ch)
    {
        filter[ch].setParams (filterType, cutoff, filterResonance);
    }

//    auto cutoff = (adsrDepth * adsr.getNextSample()) + filterCutoff;
//
//    DBG (cutoff);
//
//    for (int ch = 0; ch < numChannelsToProcess; ++ch)
//    {
//        lfo[ch].setFrequency (lfoFreq);
//        //cutoff = (lfoDepth * lfoOutput[ch]) + cutoff;
//        cutoff = std::clamp<float> (cutoff, 20.0f, 20000.0f);
//        filter[ch].setParams (filterType, cutoff, filterResonance);
//    }
}

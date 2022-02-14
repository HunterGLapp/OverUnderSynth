/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

//==============================================================================
TapSynthAudioProcessor::TapSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts (*this, nullptr, "Parameters", createParams())
#endif
{
    synth.addSound (new SynthSound());

    for (int i = 0; i < 5; i++)
    {
        synth.addVoice (new SynthVoice());
    }
}

TapSynthAudioProcessor::~TapSynthAudioProcessor()
{

}

//==============================================================================
const juce::String TapSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TapSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TapSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void TapSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TapSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    reverbParams.roomSize = 0.5f;
    reverbParams.width = 1.0f;
    reverbParams.damping = 0.5f;
    reverbParams.freezeMode = 0.0f;
    reverbParams.dryLevel = 1.0f;
    reverbParams.wetLevel = 0.0f;

    reverb.setParameters (reverbParams);
}

void TapSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TapSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    setParams();

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
    juce::dsp::AudioBlock<float> block { buffer };
    reverb.process (juce::dsp::ProcessContextReplacing<float> (block));

    meter.processRMS (buffer);
    meter.processPeak (buffer);
}

//==============================================================================
bool TapSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapSynthAudioProcessor::createEditor()
{
    return new TapSynthAudioProcessorEditor (*this);
}

//==============================================================================
void TapSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapSynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout TapSynthAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // OSC select
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("OSC1", "Oscillator 1", juce::StringArray { "Sine", "Saw", "Square" }, 0));

    // OSC Gain
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1GAIN", "Oscillator 1 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, 0.1f, "dB"));

    // OSC Pitch val
    params.push_back (std::make_unique<juce::AudioParameterInt>("OSC1PITCH", "Oscillator 1 Pitch", -48, 48, 0));

    // FM Osc Freq
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1FMFREQ", "Oscillator 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.1f }, 0.0f, "Hz"));

    //SubOscillator Gain
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB1GAIN", "Subharmonic 1 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB2GAIN", "Subharmonic 2 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB3GAIN", "Subharmonic 3 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB4GAIN", "Subharmonic 4 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB5GAIN", "Subharmonic 5 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB6GAIN", "Subharmonic 6 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB7GAIN", "Subharmonic 7 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB8GAIN", "Subharmonic 8 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUB9GAIN", "Subharmonic 9 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, -20.0f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV1GAIN", "Overtone 1 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV2GAIN", "Overtone 2 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV3GAIN", "Overtone 3 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV4GAIN", "Overtone 4 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV5GAIN", "Overtone 5 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV6GAIN", "Overtone 6 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV7GAIN", "Overtone 7 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV8GAIN", "Overtone 8 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OV9GAIN", "Overtone 9 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, -20.0f, "dB"));



    // FM Osc Depth
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1FMDEPTH", "Oscillator 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f }, 0.0f, ""));

    // LFO
    params.push_back (std::make_unique<juce::AudioParameterFloat>("LFO1FREQ", "LFO1 Frequency", juce::NormalisableRange<float> { 0.0f, 20.0f, 0.1f }, 0.0f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("LFO1DEPTH", "LFO1 Depth", juce::NormalisableRange<float> { 0.0f, 10000.0f, 0.1f, 0.3f }, 0.0f, ""));

    //Filter
    params.push_back (std::make_unique<juce::AudioParameterChoice>("FILTERTYPE", "Filter Type", juce::StringArray { "Low Pass", "Band Pass", "High Pass" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERCUTOFF", "Filter Cutoff", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 20000.0f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERRESONANCE", "Filter Resonance", juce::NormalisableRange<float> { 0.1f, 2.0f, 0.1f }, 0.1f, ""));

    // ADSR
    params.push_back (std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f }, 0.4f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERADSRDEPTH", "Filter ADSR Depth", juce::NormalisableRange<float> { 0.0f, 10000.0f, 0.1f, 0.3f }, 10000.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERATTACK", "Filter Attack", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.01f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERDECAY", "Filter Decay", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERSUSTAIN", "Filter Sustain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERRELEASE", "Filter Release", juce::NormalisableRange<float> { 0.0f, 3.0f, 0.1f }, 0.1f));

    // Reverb
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBSIZE", "Reverb Size", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBWIDTH", "Reverb Width", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDAMPING", "Reverb Damping", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.5f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDRY", "Reverb Dry", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBWET", "Reverb Wet", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBFREEZE", "Reverb Freeze", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));

    return { params.begin(), params.end() };
}

void TapSynthAudioProcessor::setParams()
{
    setVoiceParams();
    setFilterParams();
    setReverbParams();
}

void TapSynthAudioProcessor::setVoiceParams()
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            auto& attack = *apvts.getRawParameterValue ("ATTACK");
            auto& decay = *apvts.getRawParameterValue ("DECAY");
            auto& sustain = *apvts.getRawParameterValue ("SUSTAIN");
            auto& release = *apvts.getRawParameterValue ("RELEASE");

            auto& osc1Choice = *apvts.getRawParameterValue ("OSC1");

            auto& osc1Gain = *apvts.getRawParameterValue ("OSC1GAIN");

            auto& osc1Pitch = *apvts.getRawParameterValue ("OSC1PITCH");

            auto& osc1FmFreq = *apvts.getRawParameterValue ("OSC1FMFREQ");

            auto& osc1FmDepth = *apvts.getRawParameterValue ("OSC1FMDEPTH");


            auto& sub1Gain = *apvts.getRawParameterValue("SUB1GAIN");
            auto& sub2Gain = *apvts.getRawParameterValue("SUB2GAIN");
            auto& sub3Gain = *apvts.getRawParameterValue("SUB3GAIN");
            auto& sub4Gain = *apvts.getRawParameterValue("SUB4GAIN");
            auto& sub5Gain = *apvts.getRawParameterValue("SUB5GAIN");
            auto& sub6Gain = *apvts.getRawParameterValue("SUB6GAIN");
            auto& sub7Gain = *apvts.getRawParameterValue("SUB7GAIN");
            auto& sub8Gain = *apvts.getRawParameterValue("SUB8GAIN");
            auto& sub9Gain = *apvts.getRawParameterValue("SUB9GAIN");
            auto& ov1Gain = *apvts.getRawParameterValue("OV1GAIN");
            auto& ov2Gain = *apvts.getRawParameterValue("OV2GAIN");
            auto& ov3Gain = *apvts.getRawParameterValue("OV3GAIN");
            auto& ov4Gain = *apvts.getRawParameterValue("OV4GAIN");
            auto& ov5Gain = *apvts.getRawParameterValue("OV5GAIN");
            auto& ov6Gain = *apvts.getRawParameterValue("OV6GAIN");
            auto& ov7Gain = *apvts.getRawParameterValue("OV7GAIN");
            auto& ov8Gain = *apvts.getRawParameterValue("OV8GAIN");
            auto& ov9Gain = *apvts.getRawParameterValue("OV9GAIN");

            auto& filterAttack = *apvts.getRawParameterValue ("FILTERATTACK");
            auto& filterDecay = *apvts.getRawParameterValue ("FILTERDECAY");
            auto& filterSustain = *apvts.getRawParameterValue ("FILTERSUSTAIN");
            auto& filterRelease = *apvts.getRawParameterValue ("FILTERRELEASE");

            auto& osc1 = voice->getOscillator1();
            auto& sub1 = voice->getSub1();
            auto& sub2 = voice->getSub2();
            auto& sub3 = voice->getSub3();
            auto& sub4 = voice->getSub4();
            auto& sub5 = voice->getSub5();
            auto& sub6 = voice->getSub6();
            auto& sub7 = voice->getSub7();
            auto& sub8 = voice->getSub8();
            auto& sub9 = voice->getSub9();
            auto& ov1  = voice->getOv1();
            auto& ov2  = voice->getOv2();
            auto& ov3  = voice->getOv3();
            auto& ov4  = voice->getOv4();
            auto& ov5  = voice->getOv5();
            auto& ov6  = voice->getOv6();
            auto& ov7  = voice->getOv7();
            auto& ov8  = voice->getOv8();
            auto& ov9  = voice->getOv9();


            auto& adsr = voice->getAdsr();
            auto& filterAdsr = voice->getFilterAdsr();

            for (int i = 0; i < getTotalNumOutputChannels(); i++)
            {
                osc1[i].setParams (osc1Choice, osc1Gain, osc1Pitch, osc1FmFreq, osc1FmDepth);
                sub1[i].setParams (osc1Choice, sub1Gain, 0, 0, 0);
                sub2[i].setParams (osc1Choice, sub2Gain, 0, 0, 0);
                sub3[i].setParams (osc1Choice, sub3Gain, 0, 0, 0);
                sub4[i].setParams (osc1Choice, sub4Gain, 0, 0, 0);
                sub5[i].setParams (osc1Choice, sub5Gain, 0, 0, 0);
                sub6[i].setParams (osc1Choice, sub6Gain, 0, 0, 0);
                sub7[i].setParams (osc1Choice, sub7Gain, 0, 0, 0);
                sub8[i].setParams (osc1Choice, sub8Gain, 0, 0, 0);
                sub9[i].setParams (osc1Choice, sub9Gain, 0, 0, 0);
                ov1[i].setParams  (osc1Choice, ov1Gain, 0, 0, 0 );
                ov2[i].setParams  (osc1Choice, ov2Gain, 0, 0, 0 );
                ov3[i].setParams  (osc1Choice, ov3Gain, 0, 0, 0 );
                ov4[i].setParams  (osc1Choice, ov4Gain, 0, 0, 0 );
                ov5[i].setParams  (osc1Choice, ov5Gain, 0, 0, 0 );
                ov6[i].setParams  (osc1Choice, ov6Gain, 0, 0, 0 );
                ov7[i].setParams  (osc1Choice, ov7Gain, 0, 0, 0 );
                ov8[i].setParams  (osc1Choice, ov8Gain, 0, 0, 0 );
                ov9[i].setParams  (osc1Choice, ov9Gain, 0, 0, 0 );




            }

            adsr.update (attack.load(), decay.load(), sustain.load(), release.load());
            filterAdsr.update (filterAttack, filterDecay, filterSustain, filterRelease);
        }
    }
}

void TapSynthAudioProcessor::setFilterParams()
{
    auto& filterType = *apvts.getRawParameterValue ("FILTERTYPE");
    auto& filterCutoff = *apvts.getRawParameterValue ("FILTERCUTOFF");
    auto& filterResonance = *apvts.getRawParameterValue ("FILTERRESONANCE");
    auto& adsrDepth = *apvts.getRawParameterValue ("FILTERADSRDEPTH");
    auto& lfoFreq = *apvts.getRawParameterValue ("LFO1FREQ");
    auto& lfoDepth = *apvts.getRawParameterValue ("LFO1DEPTH");

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->updateModParams (filterType, filterCutoff, filterResonance, adsrDepth, lfoFreq, lfoDepth);
        }
    }
}

void TapSynthAudioProcessor::setReverbParams()
{
    reverbParams.roomSize = *apvts.getRawParameterValue ("REVERBSIZE");
    reverbParams.width = *apvts.getRawParameterValue ("REVERBWIDTH");
    reverbParams.damping = *apvts.getRawParameterValue ("REVERBDAMPING");
    reverbParams.dryLevel = *apvts.getRawParameterValue ("REVERBDRY");
    reverbParams.wetLevel = *apvts.getRawParameterValue ("REVERBWET");
    reverbParams.freezeMode = *apvts.getRawParameterValue ("REVERBFREEZE");

    reverb.setParameters (reverbParams);
}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <list>

//==============================================================================
TapSynthAudioProcessorEditor::TapSynthAudioProcessorEditor (TapSynthAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, osc1 (audioProcessor.apvts, "OSC1", "OSC1GAIN", "OSC1PITCH", "OSC1FMFREQ", "OSC1FMDEPTH")
, sub1 (audioProcessor.apvts, "SUB1", "SUB1GAIN")
, sub2 (audioProcessor.apvts, "SUB2", "SUB2GAIN")
, sub3 (audioProcessor.apvts, "SUB3", "SUB3GAIN")
, sub4 (audioProcessor.apvts, "SUB4", "SUB4GAIN")
, sub5 (audioProcessor.apvts, "SUB5", "SUB5GAIN")
, sub6 (audioProcessor.apvts, "SUB6", "SUB6GAIN")
, sub7 (audioProcessor.apvts, "SUB7", "SUB7GAIN")
, sub8 (audioProcessor.apvts, "SUB8", "SUB8GAIN")
, sub9 (audioProcessor.apvts, "SUB9", "SUB9GAIN")
, ov1 (audioProcessor.apvts, "OV1", "OV1GAIN")
, ov2 (audioProcessor.apvts, "OV2", "OV2GAIN")
, ov3 (audioProcessor.apvts, "OV3", "OV3GAIN")
, ov4 (audioProcessor.apvts, "OV4", "OV4GAIN")
, ov5 (audioProcessor.apvts, "OV5", "OV6GAIN")
, ov6 (audioProcessor.apvts, "OV6", "OV6GAIN")
, ov7 (audioProcessor.apvts, "OV7", "OV7GAIN")
, ov8 (audioProcessor.apvts, "OV8", "OV8GAIN")
, ov9 (audioProcessor.apvts, "OV9", "OV9GAIN")
, filter (audioProcessor.apvts, "FILTERTYPE", "FILTERCUTOFF", "FILTERRESONANCE")
, adsr (audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE")
, lfo1 (audioProcessor.apvts, "LFO1FREQ", "LFO1DEPTH")
, filterAdsr (audioProcessor.apvts, "FILTERATTACK", "FILTERDECAY", "FILTERSUSTAIN", "FILTERRELEASE")
, reverb (audioProcessor.apvts, "REVERBSIZE", "REVERBDAMPING", "REVERBWIDTH", "REVERBDRY", "REVERBWET", "REVERBFREEZE")
, meter (audioProcessor)
{
    auto tapImage = juce::ImageCache::getFromMemory (BinaryData::tapLogo_png, BinaryData::tapLogo_pngSize);

    if (tapImage.isValid())
        logo.setImage (tapImage, juce::RectanglePlacement::stretchToFit);
    else
        jassertfalse;

    addAndMakeVisible (osc1);

    addAndMakeVisible (sub1);
    addAndMakeVisible (sub2);
    addAndMakeVisible (sub3);
    addAndMakeVisible (sub4);
    addAndMakeVisible (sub5);
    addAndMakeVisible (sub6);
    addAndMakeVisible (sub7);
    addAndMakeVisible (sub8);
    addAndMakeVisible (sub9);
    addAndMakeVisible (ov1);
    addAndMakeVisible (ov2);
    addAndMakeVisible (ov3);
    addAndMakeVisible (ov4);
    addAndMakeVisible (ov5);
    addAndMakeVisible (ov6);
    addAndMakeVisible (ov7);
    addAndMakeVisible (ov8);
    addAndMakeVisible (ov9);
    //addAndMakeVisible (filter);
    //addAndMakeVisible (adsr);
    //addAndMakeVisible (lfo1);
    //addAndMakeVisible (filterAdsr);
    addAndMakeVisible (reverb);
    addAndMakeVisible (meter);
    //addAndMakeVisible (logo);

    osc1.setName ("Oscillator 1");
    //numSubHarms = 9;
    //numOvertones = 9
    //HarmonicComponent subHarms[] = {sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8, sub9};
    //HarmonicComponent overtones[] = {ov1, ov2, ov3, ov4, ov5, ov6, ov7, ov8, ov9};
    //for (int i = 0; i < numSubHarms, i ++)
    //{
    //    subHarms[i].setName(str(-(i+1));
    //}
    sub1.setName("-1");
    sub2.setName("-2");
    sub3.setName("-3");
    sub4.setName("-4");
    sub5.setName("-5");
    sub6.setName("-6");
    sub7.setName("-7");
    sub8.setName("-8");
    sub9.setName("-9");
    ov1.setName("1");
    ov2.setName("2");
    ov3.setName("3");
    ov4.setName("4");
    ov5.setName("5");
    ov6.setName("6");
    ov7.setName("7");
    ov8.setName("8");
    ov9.setName("9");

    //filter.setName ("Filter");
    //lfo1.setName ("Filter LFO");
    filterAdsr.setName ("Filter ADSR");
    adsr.setName ("ADSR");
    meter.setName ("Meter");

    auto oscColour = juce::Colour::fromRGB (247, 190, 67);
    auto filterColour = juce::Colour::fromRGB (246, 87, 64);

    osc1.setBoundsColour (oscColour);

    //sub1.setBoundsColour(oscColour);
    filterAdsr.setBoundsColour (filterColour);
    filter.setBoundsColour (filterColour);
    lfo1.setBoundsColour (filterColour);


    startTimerHz (30);
    setSize (2000, 525);
}

TapSynthAudioProcessorEditor::~TapSynthAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void TapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void TapSynthAudioProcessorEditor::resized()
{
    const auto oscWidth = 1000;
    const auto oscHeight = 150;
    osc1.setBounds (0, 0, oscWidth / 2, 150);
    sub1.setBounds (0, osc1.getBottom(), oscWidth / 9, oscHeight);
    sub2.setBounds(sub1.getRight(),osc1.getBottom(), oscWidth / 9, oscHeight);
    sub3.setBounds(sub2.getRight(), osc1.getBottom(), oscWidth / 9, oscHeight);
    sub4.setBounds(sub3.getRight(), osc1.getBottom(), oscWidth / 9, oscHeight);
    sub5.setBounds(sub4.getRight(), osc1.getBottom(), oscWidth / 9, oscHeight);
    sub6.setBounds(sub5.getRight(), osc1.getBottom(), oscWidth / 9, oscHeight);
    sub7.setBounds(sub6.getRight(), osc1.getBottom(), oscWidth / 9, oscHeight);
    sub8.setBounds(sub7.getRight(), osc1.getBottom(), oscWidth / 9, oscHeight);
    sub9.setBounds(sub8.getRight(), osc1.getBottom(), oscWidth / 9, oscHeight);
    ov1.setBounds(0, sub1.getBottom(), oscWidth / 9, oscHeight);
    ov2.setBounds(ov1.getRight(), sub2.getBottom(), oscWidth / 9, oscHeight);
    ov3.setBounds(ov2.getRight(), sub3.getBottom(), oscWidth / 9, oscHeight);
    ov4.setBounds(ov3.getRight(), sub4.getBottom(), oscWidth / 9, oscHeight);
    ov5.setBounds(ov4.getRight(), sub5.getBottom(), oscWidth / 9, oscHeight);
    ov6.setBounds(ov5.getRight(), sub6.getBottom(), oscWidth / 9, oscHeight);
    ov7.setBounds(ov6.getRight(), sub7.getBottom(), oscWidth / 9, oscHeight);
    ov8.setBounds(ov7.getRight(), sub8.getBottom(), oscWidth / 9, oscHeight);
    ov9.setBounds(ov8.getRight(), sub9.getBottom(), oscWidth / 9, oscHeight);
    //filter.setBounds (osc1.getRight(), 0, 180, 200);
    //lfo1.setBounds (sub9.getRight(), filter.getBottom(), 180, 160);
    //filterAdsr.setBounds (filter.getRight(), 0, 230, 360);
    //adsr.setBounds (filterAdsr.getRight(), 0, 230, 360);
    reverb.setBounds (osc1.getRight(), 0, oscWidth / 2, 150);
    meter.setBounds (reverb.getRight(), sub2.getBottom(), filterAdsr.getWidth() + lfo1.getWidth(), 150);
    //logo.setBounds (meter.getRight(), sub2.getBottom() + 30, 250, 100);
}

void TapSynthAudioProcessorEditor::timerCallback()
{
    repaint();
}

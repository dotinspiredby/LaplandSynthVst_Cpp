/*
  ==============================================================================

    SynthSound.h
    Created: 3 Jan 2022 2:35:28am
    Author:  garfi

  ==============================================================================
*/

#pragma once

#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    /*Virtual functions, which need to be accomplished*/
    virtual bool    appliesToNote(int midiNoteNumber) override { return true; }
    virtual bool 	appliesToChannel(int midiChannel) override { return true; }
};
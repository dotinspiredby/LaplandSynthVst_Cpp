/*
  ==============================================================================

    SynthVoice.h
    Created: 3 Jan 2022 2:35:16am
    Author:  garfi

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    virtual bool 	canPlaySound(juce::SynthesiserSound* sound) override;
    virtual void 	startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    virtual void 	stopNote(float velocity, bool allowTailOff) override;
    void            updateKeyFreq(double midiKeyFreq);
    void            updateNoiseCleaningLevel(float cleaningLevel);
    void            updateADSR(float a, float d, float s, float r);
    void            updateVolume(float volume);
    virtual void 	pitchWheelMoved(int newPitchWheelValue) override;
    virtual void 	controllerMoved(int controllerNumber, int newControllerValue) override;
    void            prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    virtual void 	renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples) override;
    bool            isBusy(){ return &busy; }
private:
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> bpFilter;

    float lastSampleRate; //set in preparetoplay
    float lastKeyFreq; //set in updateKeyFreq
    float lastCleaningLevel; //set in updateNoiseCleaning

    juce::Random random;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParameters;
    juce::dsp::Gain <float> gain;

    bool busy{ false };
    juce::AudioBuffer<float> laplandBuffer;
};
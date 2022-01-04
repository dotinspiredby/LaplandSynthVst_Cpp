/*
  ==============================================================================

    SynthVoice.cpp
    Created: 3 Jan 2022 2:35:16am
    Author:  garfi

  ==============================================================================
*/

#include "SynthVoice.h"


bool 	SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void 	SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    juce::MidiMessage msg;
    updateKeyFreq(msg.getMidiNoteInHertz(midiNoteNumber));
    adsr.noteOn();
    busy = true;
}

void 	SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    //updateKeyFreq(20.0);
    adsr.noteOff();
    if (!allowTailOff || !adsr.isActive()) { clearCurrentNote(); busy = false;
    }
    if (!adsr.isActive()) { busy = false; }
}

void    SynthVoice::updateKeyFreq(double midiKeyFreq)
{
    lastKeyFreq = float(midiKeyFreq);
    *bpFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, lastKeyFreq, lastCleaningLevel);
}

void    SynthVoice::updateNoiseCleaningLevel(float cleaningLevel)
{
    lastCleaningLevel = cleaningLevel;
    *bpFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, lastKeyFreq, lastCleaningLevel);

}

void    SynthVoice::updateADSR(float a, float d, float s, float r)
{
    adsrParameters.attack = a;
    adsrParameters.decay = d;
    adsrParameters.sustain = s;
    adsrParameters.release = r;

    adsr.setParameters(adsrParameters);

}

void SynthVoice::updateVolume(float volume)
{
    gain.setGainLinear(volume);
}

void 	SynthVoice::pitchWheelMoved(int newPitchWheelValue) {}
void 	SynthVoice::controllerMoved(int controllerNumber, int newControllerValue) {}


void    SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::dsp::ProcessSpec spec;
    lastSampleRate = sampleRate;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;

    bpFilter.prepare(spec);
    gain.prepare(spec);
    //gain.setGainLinear(0.01f);
    bpFilter.reset();
    updateKeyFreq(20.0);
}

void 	SynthVoice::renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive()) { return; }

    laplandBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);

    laplandBuffer.clear();

    for (int channel = 0; channel < laplandBuffer.getNumChannels(); ++channel)
    {
        float* buffer_ = laplandBuffer.getWritePointer(channel);
        for (int sample = 0; sample < laplandBuffer.getNumSamples(); ++sample)
        {
            buffer_[sample] = (random.nextFloat() * 2.0f - 1.0f) * 0.01f;
        }
    }

    juce::dsp::AudioBlock<float> block(laplandBuffer);

    /*
    code from pluginprocessor

        for (const auto& meta : midiMessages)
    {
        const auto msg = meta.getMessage();

        const auto timestamp = meta.samplePosition;

        auto mtof = msg.getMidiNoteInHertz(msg.getNoteNumber());

        for (int i = 0; i < lapland.getNumVoices(); ++i)
        {
            if (auto voice = dynamic_cast<SynthVoice*>(lapland.getVoice(i)))
            {
                voice->updateKeyFreq(mtof);
            }
        }
    }
    */
    gain.process(juce::dsp::ProcessContextReplacing<float>(block));
    bpFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    adsr.applyEnvelopeToBuffer(laplandBuffer, 0, laplandBuffer.getNumSamples());
    

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, laplandBuffer, channel, 0, numSamples);
        if (!adsr.isActive()) { clearCurrentNote(); busy = false; }
    }
}
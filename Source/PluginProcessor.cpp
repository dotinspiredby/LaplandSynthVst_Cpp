/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LaplandAudioProcessor::LaplandAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), apvts(*this, nullptr), bpFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1))
#endif
{
    
    lapland.addSound(new SynthSound);
    for (int i = 0; i < 22; i++)
    {
        lapland.addVoice(new SynthVoice());
    }
    

    juce::NormalisableRange<float> keyFreqRange(20.0f, 20000.0f, 0.1f);
    juce::NormalisableRange<float> noiseCleaningRange(20.0f, 1000.0f, 0.1f);

    juce::NormalisableRange<float> attackRange(0.1f, 1.0f, 0.01f, 0.6f);
    juce::NormalisableRange<float> decayRange(0.1f, 1.0f, 0.01f, 0.4f);
    juce::NormalisableRange<float> sustainRange(0.1f, 1.0f, 0.01f, 0.7f);
    juce::NormalisableRange<float> releaseRange(0.1f, 10.0f, 0.01f, 0.9f);

    juce::NormalisableRange<float> volumeRange(0.0f, 1.0f, 0.001f);

    apvts.createAndAddParameter("KeyFreq", "Key Frequency", "KeyFreq", keyFreqRange, 20.0f, nullptr, nullptr);
    apvts.createAndAddParameter("CleaningLevel", "Noise Cleaning Level", "CleaningLevel", noiseCleaningRange, 1000.0f, nullptr, nullptr);

    apvts.createAndAddParameter("Attack", "Attack", "Attack", attackRange, 0.1f, nullptr, nullptr);
    apvts.createAndAddParameter("Decay", "Decay", "Decay", decayRange, 0.1f, nullptr, nullptr);
    apvts.createAndAddParameter("Sustain", "Sustain", "Sustain", sustainRange, 0.1f, nullptr, nullptr);
    apvts.createAndAddParameter("Release", "Release", "Release", releaseRange, 0.6f, nullptr, nullptr);

    apvts.createAndAddParameter("Volume", "Volume", "Volume", volumeRange, 0.06f, nullptr, nullptr);
}

LaplandAudioProcessor::~LaplandAudioProcessor()
{
}

//==============================================================================
const juce::String LaplandAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LaplandAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool LaplandAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool LaplandAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double LaplandAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LaplandAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LaplandAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LaplandAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LaplandAudioProcessor::getProgramName(int index)
{
    return {};
}

void LaplandAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void LaplandAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    lapland.setCurrentPlaybackSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    //lastSampleRate = sampleRate;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    /*
    
    bpFilter.prepare(spec);
    gain.prepare(spec);
    bpFilter.reset();
    updateKeyFreq(20.0);
    updateVolume();
    */
    for (int i = 0; i < lapland.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(lapland.getVoice(i))) //returns Syntheseiser Voice,
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }

    updateVolume();
}

void LaplandAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LaplandAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void LaplandAudioProcessor::updateNoiseCleaningLevel()
{
    float cleaningLevel = *apvts.getRawParameterValue("CleaningLevel");

    *bpFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, lastKeyFreq, cleaningLevel);

    for (int i = 0; i < lapland.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(lapland.getVoice(i)))
        {
            voice->updateNoiseCleaningLevel(cleaningLevel);
            //for all voices
        }
    }
}

void LaplandAudioProcessor::updateKeyFreq(double midiKeyFreq)
{
    /*
    float keyfreq = *apvts.getRawParameterValue("KeyFreq");
    */
    lastKeyFreq = float(midiKeyFreq);
    //float cleaningLevel = *apvts.getRawParameterValue("CleaningLevel"); //redundant

    //*bpFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, lastKeyFreq, cleaningLevel);

    for (int i = 0; i < lapland.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(lapland.getVoice(i)))
        {
            voice->updateKeyFreq(lastKeyFreq);
            //if voice is NOT busy
        }
    }
}

void LaplandAudioProcessor::updateADSR()
{
    /*
    
    adsrParameters.attack = *apvts.getRawParameterValue("Attack");
    adsrParameters.decay = *apvts.getRawParameterValue("Decay");
    adsrParameters.sustain = *apvts.getRawParameterValue("Sustain");
    adsrParameters.release = *apvts.getRawParameterValue("Release");

    adsr.setParameters(adsrParameters);
    */


    auto& attack = *apvts.getRawParameterValue("Attack");
    auto& decay = *apvts.getRawParameterValue("Decay");
    auto& sustain = *apvts.getRawParameterValue("Sustain");
    auto& release = *apvts.getRawParameterValue("Release");

    for (int i = 0; i < lapland.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(lapland.getVoice(i)))
        {
            voice->updateADSR(attack.load(), decay.load(), sustain.load(), release.load());
        }
    }
}

void LaplandAudioProcessor::updateVolume()
{
    float volume = *apvts.getRawParameterValue("Volume");

    for (int i = 0; i < lapland.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(lapland.getVoice(i)))
        {
            voice->updateVolume(volume);
        }
    }

    //gain.setGainLinear(volume);
}

void LaplandAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateADSR();
    updateNoiseCleaningLevel();
    updateVolume();

    lapland.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);

   
    for (const auto& meta : midiMessages)
    {
        const auto msg = meta.getMessage();

        const auto timestamp = meta.samplePosition;

        auto mtof = msg.getMidiNoteInHertz(msg.getNoteNumber());

        for (int i = 0; i < lapland.getNumVoices(); ++i)
        {
            if (auto voice = dynamic_cast<SynthVoice*>(lapland.getVoice(i)))
            {
                if (voice->isBusy() == false) {
                    voice->updateKeyFreq(mtof);
                } 
            }
        }
    }
    /*
    
    for (const auto& meta : midiMessages)
    {
        const auto msg = meta.getMessage();

        const auto timestamp = meta.samplePosition;

        auto mtof = msg.getMidiNoteInHertz(msg.getNoteNumber());


        if (msg.isNoteOff())
        {
            adsr.noteOff();
        }
        else {
            adsr.noteOn();
        }

        updateKeyFreq(mtof);
    }
    */
    //gain.process(juce::dsp::ProcessContextReplacing<float>(block));
    //adsr.applyEnvelopeToBuffer(buffer, 0, buffer.getNumSamples());
    //bpFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool LaplandAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LaplandAudioProcessor::createEditor()
{
    return new LaplandAudioProcessorEditor(*this);
}

//==============================================================================
void LaplandAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LaplandAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaplandAudioProcessor();
}


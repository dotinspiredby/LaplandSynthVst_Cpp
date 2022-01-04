/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class LaplandAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    LaplandAudioProcessorEditor(LaplandAudioProcessor&);
    ~LaplandAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void setSlider(juce::Slider& slider, float min_value, float max_value, float valueToSet);
    void setLabel(juce::Label& label);

private:
    juce::Slider keyFreqSlider;
    juce::Slider cleaningNoiseSlider;

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    juce::Slider volumeSlider;


    juce::Label keyFreqLabel{ "Key Frequency", "Key Frequency" };
    juce::Label cleaningNoiseLabel{ "Noise Cleaning Level", "Noise Cleaning Level" };

    juce::Label attackLabel{ "Attack", "Attack" };
    juce::Label decayLabel{ "Decay", "Decay" };
    juce::Label sustainLabel{ "Sustain", "Sustain" };
    juce::Label releaseLabel{ "Release", "Release" };

    juce::Label volumeLabel{ "Volume", "Volume" };

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<Attachment> keyFreqSliderAttch;
    std::unique_ptr<Attachment> cleaningNoiseSliderAttch;

    std::unique_ptr<Attachment> attackAttch;
    std::unique_ptr<Attachment> decayAttch;
    std::unique_ptr<Attachment> sustainAttch;
    std::unique_ptr<Attachment> releaseAttch;

    std::unique_ptr<Attachment> volumeAttch;

    LaplandAudioProcessor& audioProcessor;

    juce::ImageComponent imgComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaplandAudioProcessorEditor)
};

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LaplandAudioProcessorEditor::LaplandAudioProcessorEditor(LaplandAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    auto image = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

    if (!image.isNull())
    {
        imgComponent.setImage(image, juce::RectanglePlacement::stretchToFit);
    }
    else {
        jassert(!image.isNull());
    }
    addAndMakeVisible(imgComponent);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 400);

    setSlider(keyFreqSlider, 20.0f, 20000.0f, 1000.0f);
    setSlider(cleaningNoiseSlider, 20.0f, 1000.0f, 1000.0f);

    setLabel(keyFreqLabel);
    setLabel(cleaningNoiseLabel);

    keyFreqSliderAttch = std::make_unique<Attachment>(audioProcessor.apvts, "KeyFreq", keyFreqSlider);
    cleaningNoiseSliderAttch = std::make_unique<Attachment>(audioProcessor.apvts, "CleaningLevel", cleaningNoiseSlider);


    setSlider(attackSlider, 0.1f, 1.0f, 0.1f);
    setSlider(decaySlider, 0.1f, 1.0f, 0.1f);
    setSlider(sustainSlider, 0.1f, 1.0f, 0.1f);
    setSlider(releaseSlider, 0.1f, 3.0f, 0.1f);

    setLabel(attackLabel);
    setLabel(decayLabel);
    setLabel(sustainLabel);
    setLabel(releaseLabel);

    attackAttch = std::make_unique<Attachment>(audioProcessor.apvts, "Attack", attackSlider);
    decayAttch = std::make_unique<Attachment>(audioProcessor.apvts, "Decay", decaySlider);
    sustainAttch = std::make_unique<Attachment>(audioProcessor.apvts, "Sustain", sustainSlider);
    releaseAttch = std::make_unique<Attachment>(audioProcessor.apvts, "Release", releaseSlider);

    setSlider(volumeSlider, 0.0f, 0.09f, 0.06f);
    setLabel(volumeLabel);
    volumeAttch = std::make_unique<Attachment>(audioProcessor.apvts, "Volume", volumeSlider);
}

LaplandAudioProcessorEditor::~LaplandAudioProcessorEditor()
{
}

//==============================================================================
void LaplandAudioProcessorEditor::paint(juce::Graphics& g)
{

    juce::Colour coldgreen(0, 115, 105);
    juce::Colour darkpink(135, 0, 95);

    juce::ColourGradient gradient (coldgreen, 20.0f, 20.0f, darkpink, 400.0f, 400.0f, false);
    g.setGradientFill(gradient);
    g.fillAll();

    g.setColour(juce::Colours::cadetblue);
    g.setFont(15.0f);

}

void LaplandAudioProcessorEditor::resized()
{
    const auto SliderSide = 180;
    const auto padding = 10;
    const auto sliderStartX = 10;
    const auto Y = 25;

    const auto bounds = getLocalBounds().reduced(10);
    const auto sliderWidth = bounds.getWidth() / 4 - padding;
    const auto sliderHeight = bounds.getWidth() / 4 - padding;


    cleaningNoiseSlider.setBounds(110, Y, SliderSide, SliderSide);
    cleaningNoiseLabel.setBounds(cleaningNoiseSlider.getX(), cleaningNoiseSlider.getY() - 15, SliderSide, 20);

    volumeSlider.setBounds(cleaningNoiseSlider.getRight(), SliderSide - 60, sliderWidth, sliderHeight);
    volumeLabel.setBounds(cleaningNoiseSlider.getRight(), SliderSide - 75, sliderWidth, 20);

    auto ADSR_Y = cleaningNoiseSlider.getY() + 240;

    attackSlider.setBounds(15, ADSR_Y, sliderWidth, sliderHeight);
    attackLabel.setBounds(15, attackSlider.getY() - 15, sliderWidth, 20);

    decaySlider.setBounds((attackSlider.getRight() + padding), ADSR_Y, sliderWidth, sliderHeight);
    decayLabel.setBounds(decaySlider.getX(), attackSlider.getY() - 15, sliderWidth, 20);

    sustainSlider.setBounds(decaySlider.getRight() + padding, ADSR_Y, sliderWidth, sliderHeight);
    sustainLabel.setBounds(sustainSlider.getX(), attackSlider.getY() - 15, sliderWidth, 20);

    releaseSlider.setBounds(sustainSlider.getRight() + padding, ADSR_Y, sliderWidth, sliderHeight);
    releaseLabel.setBounds(releaseSlider.getX(), attackSlider.getY() - 15, sliderWidth, 20);

    imgComponent.setBounds(-10.0f, 0, SliderSide + 90, SliderSide + 25);
}

void LaplandAudioProcessorEditor::setSlider(juce::Slider& slider, float min_value, float max_value, float valueToSet)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider.setRange(min_value, max_value);
    slider.setValue(valueToSet);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour{97, 84, 105});
    slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::cadetblue);
    slider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::lightblue);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 25);
    addAndMakeVisible(&slider);
}

void LaplandAudioProcessorEditor::setLabel(juce::Label& label)
{
    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::lightblue);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&label);
}

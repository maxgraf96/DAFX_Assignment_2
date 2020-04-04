/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

Dafx_assignment_2AudioProcessorEditor::Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize(1024, 600);

    // Get AudioParameters from processor
    auto& params = processor.getParameters();	
    AudioParameterFloat* windowLengthParam = static_cast<AudioParameterFloat*>(params.getUnchecked(0));
    AudioParameterFloat* delayTimeParam = static_cast<AudioParameterFloat*>(params.getUnchecked(1));
    AudioParameterFloat* delayFeedbackParam = static_cast<AudioParameterFloat*>(params.getUnchecked(2));
    AudioParameterFloat* delayWetParam = static_cast<AudioParameterFloat*>(params.getUnchecked(3));

    int sliderWidth = 150;
    // Assign parameters to sliders
    windowLengthSlider.reset(new Slider());
    windowLengthSlider->setBounds(12, 212, 150, 150);
    windowLengthSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    windowLengthSlider->setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    windowLengthSlider->setRange(windowLengthParam->range.start, windowLengthParam->range.end);
    windowLengthSlider->setValue(*windowLengthParam);
    windowLengthSlider->addListener(this);
    windowLengthSlider->setPopupDisplayEnabled(true, false, this);
    windowLengthSlider->setTextValueSuffix(" window length");

    delayTimeSlider.reset(new Slider());
    delayTimeSlider->setBounds(sliderWidth + 12, 212, 150, 150);
    delayTimeSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delayTimeSlider->setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    delayTimeSlider->setRange(delayTimeParam->range.start, delayTimeParam->range.end);
    delayTimeSlider->setValue(*delayTimeParam);
    delayTimeSlider->addListener(this);
    delayTimeSlider->setPopupDisplayEnabled(true, false, this);
    delayTimeSlider->setTextValueSuffix(" delay time");

    delayFeedbackSlider.reset(new Slider());
    delayFeedbackSlider->setBounds(2 * sliderWidth + 12, 212, 150, 150);
    delayFeedbackSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delayFeedbackSlider->setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    delayFeedbackSlider->setRange(delayFeedbackParam->range.start, delayFeedbackParam->range.end);
    delayFeedbackSlider->setValue(*delayFeedbackParam);
    delayFeedbackSlider->addListener(this);
    delayFeedbackSlider->setPopupDisplayEnabled(true, false, this);
    delayFeedbackSlider->setTextValueSuffix(" delay feedback");

    delayWetSlider.reset(new Slider());
    delayWetSlider->setBounds(3 * sliderWidth + 12, 212, 150, 150);
    delayWetSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delayWetSlider->setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    delayWetSlider->setRange(delayWetParam->range.start, delayWetParam->range.end);
    delayWetSlider->setValue(*delayWetParam);
    delayWetSlider->addListener(this);
    delayWetSlider->setPopupDisplayEnabled(true, false, this);
    delayWetSlider->setTextValueSuffix(" delay wet");

    addAndMakeVisible(windowLengthSlider.get());
    addAndMakeVisible(delayTimeSlider.get());
    addAndMakeVisible(delayFeedbackSlider.get());
    addAndMakeVisible(delayWetSlider.get());



    // Add child components
    samplePanel.reset(p.getSamplePanel());
    addAndMakeVisible(samplePanel.get());

    addKeyListener(this);
}

Dafx_assignment_2AudioProcessorEditor::~Dafx_assignment_2AudioProcessorEditor()
{
    samplePanel.release();
}

void Dafx_assignment_2AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void Dafx_assignment_2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void Dafx_assignment_2AudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	auto& params = processor.getParameters();
	
	// As more sliders are added, this if clause grows
	if(slider == windowLengthSlider.get())
	{
		AudioParameterFloat* windowLengthParam = static_cast<AudioParameterFloat*>(params.getUnchecked(0));
		*windowLengthParam = windowLengthSlider->getValue();
        samplePanel->setWindowLength(*windowLengthParam);
	}
    if (slider == delayFeedbackSlider.get()) {
        AudioParameterFloat* delayFeedbackParam = static_cast<AudioParameterFloat*>(params.getUnchecked(2));
        *delayFeedbackParam = delayFeedbackSlider->getValue();
        processor.setDelayFeedback(*delayFeedbackParam);
    }
    if (slider == delayWetSlider.get()) {
        AudioParameterFloat* delayWetParam = static_cast<AudioParameterFloat*>(params.getUnchecked(3));
        *delayWetParam = delayWetSlider->getValue();
        processor.setDelayWet(*delayWetParam);
    }
}

bool Dafx_assignment_2AudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    return false;
}
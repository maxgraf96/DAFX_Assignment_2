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
    AudioParameterFloat* positionParameter = static_cast<AudioParameterFloat*>(params.getUnchecked(0));

    // Assign parameters to sliders
    windowLengthSlider.reset(new Slider());
    windowLengthSlider->setBounds(12, 212, 150, 150);
    windowLengthSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    windowLengthSlider->setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    windowLengthSlider->setRange(positionParameter->range.start, positionParameter->range.end);
    windowLengthSlider->setValue(*positionParameter);
    windowLengthSlider->addListener(this);
    addAndMakeVisible(windowLengthSlider.get());

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
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
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
}

bool Dafx_assignment_2AudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    // Toggle audio playback with space key
    if (key == juce::KeyPress::spaceKey) {
        processor.togglePlaying();
        return true;
    }
    return false;
}
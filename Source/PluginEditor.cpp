/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

Dafx_assignment_2AudioProcessorEditor::Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)
{
    setSize(1024, 600);

    int sliderWidth = 150;
    
    positionSlider.setBounds(12, 212, 150, 150);
    positionSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    positionSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    positionSlider.setPopupDisplayEnabled(true, false, this);
    positionSlider.setTextValueSuffix(" sample position");

    windowLengthSlider.setBounds(sliderWidth + 12, 212, 150, 150);
    windowLengthSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    windowLengthSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    windowLengthSlider.setPopupDisplayEnabled(true, false, this);
    windowLengthSlider.setTextValueSuffix(" window length");

    delayFeedbackSlider.setBounds(2 * sliderWidth + 12, 212, 150, 150);
    delayFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delayFeedbackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    delayFeedbackSlider.setPopupDisplayEnabled(true, false, this);
    delayFeedbackSlider.setTextValueSuffix(" delay feedback");

    delayWetSlider.setBounds(3 * sliderWidth + 12, 212, 150, 150);
    delayWetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delayWetSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    delayWetSlider.setPopupDisplayEnabled(true, false, this);
    delayWetSlider.setTextValueSuffix(" delay wet");

    addAndMakeVisible(positionSlider);
    addAndMakeVisible(windowLengthSlider);
    addAndMakeVisible(delayFeedbackSlider);
    addAndMakeVisible(delayWetSlider);

    // Hook up sliders to state management
    positionAttachment.reset(new SliderAttachment(valueTreeState, "position", positionSlider));
    windowLengthAttachment.reset(new SliderAttachment(valueTreeState, "windowLength", windowLengthSlider));
    delayFeedbackAttachment.reset(new SliderAttachment(valueTreeState, "delayFeedback", delayFeedbackSlider));
    delayWetAttachment.reset(new SliderAttachment(valueTreeState, "delayWet", delayWetSlider));

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

bool Dafx_assignment_2AudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    return false;
}
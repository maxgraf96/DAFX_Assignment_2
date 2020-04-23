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

    delayFeedbackSlider.setBounds(12, positionSlider.getBounds().getBottomLeft().getY() + 24 , 150, 150);
    delayFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delayFeedbackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    delayFeedbackSlider.setPopupDisplayEnabled(true, false, this);
    delayFeedbackSlider.setTextValueSuffix(" delay feedback");

    // Mode (String vs. ADSR)
    auto toggleSize = 100;
    modeToggle.setBounds(windowLengthSlider.getBounds().getTopRight().getX() + 24, 212, toggleSize, toggleSize);
    modeToggle.setButtonText("ADSR Mode");
    modeToggle.changeWidthToFitText();
    valueTreeState.addParameterListener("mode", this);
    
    // ADSR elements
    attackSlider.setBounds(modeToggle.getBounds().getTopRight().getX() + 24, 212, 150, 150);
    attackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    attackSlider.setPopupDisplayEnabled(true, false, this);
    attackSlider.setTextValueSuffix(" ms attack");

    decaySlider.setBounds(attackSlider.getBounds().getTopRight().getX() + 12, 212, 150, 150);
    decaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    decaySlider.setPopupDisplayEnabled(true, false, this);
    decaySlider.setTextValueSuffix(" ms decay");

    sustainSlider.setBounds(attackSlider.getBounds().getBottomLeft().getX(), 
        attackSlider.getBounds().getBottomLeft().getY() + 24, 150, 150);
    sustainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    sustainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    sustainSlider.setPopupDisplayEnabled(true, false, this);
    sustainSlider.setTextValueSuffix(" sustain");

    releaseSlider.setBounds(sustainSlider.getBounds().getTopRight().getX() + 12, 
        sustainSlider.getBounds().getTopLeft().getY(), 150, 150);
    releaseSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    releaseSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    releaseSlider.setPopupDisplayEnabled(true, false, this);
    releaseSlider.setTextValueSuffix(" ms release");

    // Hook up sliders to state management
    positionAttachment.reset(new SliderAttachment(valueTreeState, "position", positionSlider));
    windowLengthAttachment.reset(new SliderAttachment(valueTreeState, "windowLength", windowLengthSlider));
    delayFeedbackAttachment.reset(new SliderAttachment(valueTreeState, "delayFeedback", delayFeedbackSlider));
    modeAttachment.reset(new ButtonAttachment(valueTreeState, "mode", modeToggle));
    attackAttachment.reset(new SliderAttachment(valueTreeState, "attack", attackSlider));
    decayAttachment.reset(new SliderAttachment(valueTreeState, "decay", decaySlider));
    sustainAttachment.reset(new SliderAttachment(valueTreeState, "sustain", sustainSlider));
    releaseAttachment.reset(new SliderAttachment(valueTreeState, "release", releaseSlider));

    // Make nonlinear
    attackSlider.setSkewFactorFromMidPoint(500);
    releaseSlider.setSkewFactorFromMidPoint(1000);

    addAndMakeVisible(positionSlider);
    addAndMakeVisible(windowLengthSlider);
    addAndMakeVisible(delayFeedbackSlider);
    addAndMakeVisible(modeToggle);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);

    // Enable/disable sliders based on mode
    bool isADSRMode = modeToggle.getToggleStateValue().getValue();
    delayFeedbackSlider.setEnabled(!isADSRMode);

    attackSlider.setEnabled(isADSRMode);
    decaySlider.setEnabled(isADSRMode);
    sustainSlider.setEnabled(isADSRMode);
    releaseSlider.setEnabled(isADSRMode);

    // Add child components
    samplePanel.reset(p.getSamplePanel());
    addAndMakeVisible(samplePanel.get());
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

void Dafx_assignment_2AudioProcessorEditor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == "mode") {
        if (newValue == 0.0) {
            // String mode
            // Activate feedback slider
            delayFeedbackSlider.setEnabled(true);
            attackSlider.setEnabled(false);
            decaySlider.setEnabled(false);
            sustainSlider.setEnabled(false);
            releaseSlider.setEnabled(false);
            repaint();
        }
        else {
            delayFeedbackSlider.setEnabled(false);

            attackSlider.setEnabled(true);
            decaySlider.setEnabled(true);
            sustainSlider.setEnabled(true);
            releaseSlider.setEnabled(true);

            repaint();
        }
    }
}
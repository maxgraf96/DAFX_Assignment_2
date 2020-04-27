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

    auto rotarySliderSize = 150;
    
    positionSlider.setBounds(12, 212, rotarySliderSize, rotarySliderSize);
    positionSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    positionSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    positionSlider.setPopupDisplayEnabled(true, false, this);
    positionSlider.setTextValueSuffix(" sample position");

    windowLengthSlider.setBounds(rotarySliderSize + 12, 212, rotarySliderSize, rotarySliderSize);
    windowLengthSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    windowLengthSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    windowLengthSlider.setPopupDisplayEnabled(true, false, this);
    windowLengthSlider.setTextValueSuffix("\nwindow length");

    delayFeedbackSlider.setBounds(12, positionSlider.getBounds().getBottomLeft().getY() + 24 , rotarySliderSize, rotarySliderSize);
    delayFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delayFeedbackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    delayFeedbackSlider.setPopupDisplayEnabled(true, false, this);
    delayFeedbackSlider.setTextValueSuffix("\ndelay feedback");

    // Mode (String vs. ADSR)
    auto toggleSize = 24;
    modeToggle.setBounds(windowLengthSlider.getBounds().getTopRight().getX() + 24, 224, toggleSize, toggleSize);
    modeToggle.setButtonText("ADSR Mode");
    modeToggle.changeWidthToFitText();
    valueTreeState.addParameterListener("mode", this);

    // ADSR elements
    attackSlider.setBounds(modeToggle.getBounds().getTopRight().getX() + 24, 212, rotarySliderSize, rotarySliderSize);
    attackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    attackSlider.setPopupDisplayEnabled(true, false, this);
    attackSlider.setTextValueSuffix(" ms attack");

    decaySlider.setBounds(attackSlider.getBounds().getTopRight().getX() + 12, 212, rotarySliderSize, rotarySliderSize);
    decaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    decaySlider.setPopupDisplayEnabled(true, false, this);
    decaySlider.setTextValueSuffix(" ms decay");

    sustainSlider.setBounds(attackSlider.getBounds().getBottomLeft().getX(), 
        attackSlider.getBounds().getBottomLeft().getY() + 24, rotarySliderSize, rotarySliderSize);
    sustainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    sustainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    sustainSlider.setPopupDisplayEnabled(true, false, this);
    sustainSlider.setTextValueSuffix(" sustain");

    releaseSlider.setBounds(sustainSlider.getBounds().getTopRight().getX() + 12, 
        sustainSlider.getBounds().getTopLeft().getY(), rotarySliderSize, rotarySliderSize);
    releaseSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    releaseSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 50);
    releaseSlider.setPopupDisplayEnabled(true, false, this);
    releaseSlider.setTextValueSuffix(" ms release");

    // Fix velocity to 127 toggle
    dynamicVelocityToggle.setBounds(decaySlider.getBounds().getTopRight().getX() + 24, 224, toggleSize, toggleSize);
    dynamicVelocityToggle.setButtonText("Dynamic velocity");
    dynamicVelocityToggle.changeWidthToFitText();
    valueTreeState.addParameterListener("dynamicVelocity", this);

    // Adaptive decay toggle
    adaptiveDecayToggle.setBounds(dynamicVelocityToggle.getBounds().getTopLeft().getX(), 
        dynamicVelocityToggle.getBounds().getBottomLeft().getY() + 12, toggleSize, toggleSize);
    adaptiveDecayToggle.setButtonText("Adaptive decay time");
    adaptiveDecayToggle.changeWidthToFitText();
    valueTreeState.addParameterListener("adaptiveDecay", this);

    // Hook up sliders to state management
    positionAttachment.reset(new SliderAttachment(valueTreeState, "position", positionSlider));
    windowLengthAttachment.reset(new SliderAttachment(valueTreeState, "windowLength", windowLengthSlider));
    delayFeedbackAttachment.reset(new SliderAttachment(valueTreeState, "delayFeedback", delayFeedbackSlider));
    modeAttachment.reset(new ButtonAttachment(valueTreeState, "mode", modeToggle));
    attackAttachment.reset(new SliderAttachment(valueTreeState, "attack", attackSlider));
    decayAttachment.reset(new SliderAttachment(valueTreeState, "decay", decaySlider));
    sustainAttachment.reset(new SliderAttachment(valueTreeState, "sustain", sustainSlider));
    releaseAttachment.reset(new SliderAttachment(valueTreeState, "release", releaseSlider));
    dynamicVelocityAttachment.reset(new ButtonAttachment(valueTreeState, "dynamicVelocity", dynamicVelocityToggle));
    adaptiveDecayAttachment.reset(new ButtonAttachment(valueTreeState, "adaptiveDecay", adaptiveDecayToggle));

    // Make nonlinear
    attackSlider.setSkewFactorFromMidPoint(500);
    releaseSlider.setSkewFactorFromMidPoint(1000);

    // Set finer step size
    double interval = 0.001;
    auto delayFeedbackRange = valueTreeState.getParameterRange("delayFeedback");
    delayFeedbackSlider.setRange(delayFeedbackRange.getRange().getStart(), delayFeedbackRange.getRange().getEnd(), interval);

    addAndMakeVisible(positionSlider);
    addAndMakeVisible(windowLengthSlider);
    addAndMakeVisible(delayFeedbackSlider);
    addAndMakeVisible(modeToggle);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(dynamicVelocityToggle);
    addAndMakeVisible(adaptiveDecayToggle);

    // Enable/disable sliders based on mode
    bool isADSRMode = modeToggle.getToggleStateValue().getValue();
    delayFeedbackSlider.setEnabled(!isADSRMode);
    adaptiveDecayToggle.setEnabled(!isADSRMode);

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
    valueTreeState.removeParameterListener("mode", this);
    valueTreeState.removeParameterListener("dynamicVelocity", this);
    valueTreeState.removeParameterListener("adaptiveDecay", this);
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
        if (newValue == 0.0f) {
            // String mode
            // Activate feedback slider
            delayFeedbackSlider.setEnabled(true);
            attackSlider.setEnabled(false);
            decaySlider.setEnabled(false);
            sustainSlider.setEnabled(false);
            releaseSlider.setEnabled(false);
            adaptiveDecayToggle.setEnabled(true);
            repaint();
        }
        else {
            delayFeedbackSlider.setEnabled(false);

            attackSlider.setEnabled(true);
            decaySlider.setEnabled(true);
            sustainSlider.setEnabled(true);
            releaseSlider.setEnabled(true);
            adaptiveDecayToggle.setEnabled(false);
            repaint();
        }
    }
}
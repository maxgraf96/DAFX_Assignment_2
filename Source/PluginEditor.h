/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;


/**
*/
class Dafx_assignment_2AudioProcessorEditor  : public AudioProcessorEditor, private AudioProcessorValueTreeState::Listener
{
public:
    Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor&, AudioProcessorValueTreeState& vts);
    ~Dafx_assignment_2AudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
    Dafx_assignment_2AudioProcessor& processor;
    void parameterChanged(const String& parameterID, float newValue) override;
    
    // State management (persistence)
    AudioProcessorValueTreeState& valueTreeState;
    std::unique_ptr<SliderAttachment> positionAttachment;
    std::unique_ptr<SliderAttachment> windowLengthAttachment;
    std::unique_ptr<SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<ButtonAttachment> modeAttachment;
    std::unique_ptr<ButtonAttachment> dynamicVelocityAttachment;
    std::unique_ptr<ButtonAttachment> adaptiveDecayAttachment;

    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    // UI Elements
    // Sliders
    Slider positionSlider;
    Slider windowLengthSlider;
    Slider delayFeedbackSlider;

    // Togglebutton
    ToggleButton modeToggle;
    ToggleButton dynamicVelocityToggle;
    ToggleButton adaptiveDecayToggle;

    // ADSR elements
    Slider attackSlider;
    Slider decaySlider;
    Slider sustainSlider;
    Slider releaseSlider;

    // Panel that holds audio sample
    // Supports drag and drop
    std::unique_ptr<SamplePanel> samplePanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessorEditor)
};

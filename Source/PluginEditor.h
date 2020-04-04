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

/**
*/
class Dafx_assignment_2AudioProcessorEditor  : public AudioProcessorEditor, KeyListener
{
public:
    Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor&, AudioProcessorValueTreeState& vts);
    ~Dafx_assignment_2AudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    //void sliderValueChanged(Slider* slider) override;

private:
    Dafx_assignment_2AudioProcessor& processor;
    
    // State management (persistence)
    AudioProcessorValueTreeState& valueTreeState;
    std::unique_ptr<SliderAttachment> positionAttachment;
    std::unique_ptr<SliderAttachment> windowLengthAttachment;
    std::unique_ptr<SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<SliderAttachment> delayWetAttachment;


    // UI Elements
    // Sliders
    Slider positionSlider;
    Slider windowLengthSlider;
    Slider delayFeedbackSlider;
    Slider delayWetSlider;


    // Panel that holds audio sample
    // Supports drag and drop
    std::unique_ptr<SamplePanel> samplePanel;

    // For detecting keyboard events
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessorEditor)
};

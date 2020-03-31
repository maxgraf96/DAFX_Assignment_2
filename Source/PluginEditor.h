/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

/**
*/
class Dafx_assignment_2AudioProcessorEditor  : public AudioProcessorEditor, Slider::Listener, KeyListener
{
public:
    Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor&);
    ~Dafx_assignment_2AudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;

private:
    Dafx_assignment_2AudioProcessor& processor;

    // UI Elements
    // Sliders
    std::unique_ptr<Slider> windowLengthSlider;
    std::unique_ptr<Slider> delayTimeSlider;
    std::unique_ptr<Slider> delayFeedbackSlider;
    std::unique_ptr<Slider> delayWetSlider;


    // Panel that holds audio sample
    // Supports drag and drop
    std::unique_ptr<SamplePanel> samplePanel;

    // For detecting keyboard events
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessorEditor)
};

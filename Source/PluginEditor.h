/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "JUCEEditor.h"

/**
*/
class Dafx_assignment_2AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor&, AudioProcessorValueTreeState& vts, SamplePanel& samplePanel);
    ~Dafx_assignment_2AudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
    Dafx_assignment_2AudioProcessor& processor;
    
    // State management (persistence)
    AudioProcessorValueTreeState& valueTreeState;

    // Main editor built with JUCE GUI
    std::unique_ptr<JUCEEditor> editor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessorEditor)
};

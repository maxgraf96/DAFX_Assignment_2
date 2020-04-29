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
 *This class is currently mainly a wrapper for the JUCEEditor.
*/
class Dafx_assignment_2AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor&, AudioProcessorValueTreeState& vts, SamplePanel& samplePanel);
    ~Dafx_assignment_2AudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
	// Reference to the main audio processor
    Dafx_assignment_2AudioProcessor& processor;
    
    // State management
    AudioProcessorValueTreeState& valueTreeState;

    // Main editor built with Projucer GUI editor
    std::unique_ptr<JUCEEditor> editor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessorEditor)
};

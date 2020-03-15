/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "SamplePanel.h"

/**
*/
class Dafx_assignment_2AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor&);
    ~Dafx_assignment_2AudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
    Dafx_assignment_2AudioProcessor& processor;

    // Panel that holds audio sample
    // Supports drag and drop
    std::unique_ptr<SamplePanel> samplePanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessorEditor)
};

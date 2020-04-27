/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

Dafx_assignment_2AudioProcessorEditor::Dafx_assignment_2AudioProcessorEditor (Dafx_assignment_2AudioProcessor& p, AudioProcessorValueTreeState& vts, SamplePanel& samplePanel)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)
{
    setSize(1024, 600);

    editor.reset(new JUCEEditor(p, vts, samplePanel));
    addAndMakeVisible(*editor);
}

Dafx_assignment_2AudioProcessorEditor::~Dafx_assignment_2AudioProcessorEditor()
{
}

void Dafx_assignment_2AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void Dafx_assignment_2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
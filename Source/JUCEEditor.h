/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#ifndef JUCEEDITOR_H
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "jucefiltergraph/FilterGraph.h"
#endif

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    The main part of this class was created using the Projucer GUI editor.
	It contains all UI elements and controls, and maps them to the processing.
                                                                    //[/Comments]
*/
class JUCEEditor  : public Component,
                    private AudioProcessorValueTreeState::Listener,
                    public Slider::Listener,
                    public Button::Listener
{
public:
    //==============================================================================
    JUCEEditor (Dafx_assignment_2AudioProcessor& p, AudioProcessorValueTreeState& vts, SamplePanel& samplePanel);
    ~JUCEEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Dafx_assignment_2AudioProcessor& processor;
    // State management
    AudioProcessorValueTreeState& valueTreeState;

    // Attachments for communication with state management
    std::unique_ptr<SliderAttachment> positionAttachment;
    std::unique_ptr<SliderAttachment> windowLengthAttachment;
    std::unique_ptr<SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<ButtonAttachment> modeAttachment;
    std::unique_ptr<ButtonAttachment> dynamicVelocityAttachment;
    std::unique_ptr<ButtonAttachment> adaptiveDecayAttachment;
    std::unique_ptr<SliderAttachment> pitchBendRangeAttachment;
    std::unique_ptr<SliderAttachment> mainFilterCutoffAttachment;
    std::unique_ptr<SliderAttachment> mainFilterQAttachment;
    std::unique_ptr<SliderAttachment> mainOutputGainAttachment;

    // Panel that holds audio sample (created in AudioProcessor)
    SamplePanel& samplePanel;

	// Callback for changes made to the audio parameters
    void parameterChanged(const String& parameterID, float newValue) override;

	// The filter visualisation component
    std::unique_ptr<FilterGraph> filterGraph;

	// Necessary for enabling tooltips
    std::unique_ptr<TooltipWindow> tooltip;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> positionSlider;
    std::unique_ptr<Label> label;
    std::unique_ptr<Slider> windowLengthSlider;
    std::unique_ptr<Label> label2;
    std::unique_ptr<Slider> delayFeedbackSlider;
    std::unique_ptr<Label> label3;
    std::unique_ptr<ToggleButton> modeToggle;
    std::unique_ptr<GroupComponent> adsrGroup;
    std::unique_ptr<Slider> attackSlider;
    std::unique_ptr<Slider> decaySlider;
    std::unique_ptr<Slider> sustainSlider;
    std::unique_ptr<Slider> releaseSlider;
    std::unique_ptr<ToggleButton> dynamicVelocityToggle;
    std::unique_ptr<ToggleButton> adaptiveDecayToggle;
    std::unique_ptr<Slider> pitchBendRangeSlider;
    std::unique_ptr<Label> pitchBendRangeLabel;
    std::unique_ptr<Label> attackLabel;
    std::unique_ptr<Label> decayLabel;
    std::unique_ptr<Label> sustainLabel;
    std::unique_ptr<Label> releaseLabel;
    std::unique_ptr<Slider> mainFilterCutoffSlider;
    std::unique_ptr<Slider> mainFilterQSlider;
    std::unique_ptr<Label> filterCutoffLabel;
    std::unique_ptr<Label> filterQLabel;
    std::unique_ptr<Slider> mainOutputGainSlider;
    std::unique_ptr<Label> mainOutputGainLabel;
    std::unique_ptr<Label> mainOutputGainValLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JUCEEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]


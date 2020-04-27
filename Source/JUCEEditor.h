/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
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
    ~JUCEEditor();

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
    // State management (persistence)
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

    // Panel that holds audio sample (created in AudioProcessor)
    SamplePanel& samplePanel;

    void parameterChanged(const String& parameterID, float newValue) override;

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


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JUCEEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]


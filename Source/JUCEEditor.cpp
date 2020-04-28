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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "JUCEEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
JUCEEditor::JUCEEditor (Dafx_assignment_2AudioProcessor& p, AudioProcessorValueTreeState& vts, SamplePanel& samplePanel)
    : processor(p), valueTreeState(vts), samplePanel(samplePanel)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    positionSlider.reset (new Slider ("positionSlider"));
    addAndMakeVisible (positionSlider.get());
    positionSlider->setTooltip (TRANS("Position in the loaded sample."));
    positionSlider->setRange (0, 10, 0);
    positionSlider->setSliderStyle (Slider::LinearBar);
    positionSlider->setTextBoxStyle (Slider::TextBoxAbove, true, 80, 20);
    positionSlider->setColour (Slider::trackColourId, Colour (0x8a0a170c));
    positionSlider->setColour (Slider::textBoxTextColourId, Colours::aliceblue);
    positionSlider->addListener (this);

    label.reset (new Label ("new label",
                            TRANS("Window position")));
    addAndMakeVisible (label.get());
    label->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colours::aliceblue);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label->setBounds (20, 288, 150, 24);

    windowLengthSlider.reset (new Slider ("windowLengthSlider"));
    addAndMakeVisible (windowLengthSlider.get());
    windowLengthSlider->setTooltip (TRANS("Position in the loaded sample."));
    windowLengthSlider->setRange (0, 2400, 1);
    windowLengthSlider->setSliderStyle (Slider::LinearBar);
    windowLengthSlider->setTextBoxStyle (Slider::TextBoxAbove, true, 80, 20);
    windowLengthSlider->setColour (Slider::trackColourId, Colour (0x8a0a170c));
    windowLengthSlider->setColour (Slider::textBoxTextColourId, Colours::aliceblue);
    windowLengthSlider->addListener (this);

    label2.reset (new Label ("new label",
                             TRANS("Window length (samples)")));
    addAndMakeVisible (label2.get());
    label2->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (Label::textColourId, Colours::aliceblue);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label2->setBounds (20, 381, 236, 24);

    delayFeedbackSlider.reset (new Slider ("delayFeedbackSlider"));
    addAndMakeVisible (delayFeedbackSlider.get());
    delayFeedbackSlider->setTooltip (TRANS("Position in the loaded sample."));
    delayFeedbackSlider->setRange (0, 1, 0.0001);
    delayFeedbackSlider->setSliderStyle (Slider::LinearBar);
    delayFeedbackSlider->setTextBoxStyle (Slider::TextBoxAbove, true, 80, 20);
    delayFeedbackSlider->setColour (Slider::trackColourId, Colour (0x8a0a170c));
    delayFeedbackSlider->setColour (Slider::textBoxTextColourId, Colours::aliceblue);
    delayFeedbackSlider->addListener (this);

    label3.reset (new Label ("new label",
                             TRANS("Delay feedback")));
    addAndMakeVisible (label3.get());
    label3->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (Label::textColourId, Colours::aliceblue);
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label3->setBounds (20, 476, 150, 24);

    modeToggle.reset (new ToggleButton ("modeToggle"));
    addAndMakeVisible (modeToggle.get());
    modeToggle->setTooltip (TRANS("Enable/disable ADSR mode"));
    modeToggle->setButtonText (String());
    modeToggle->addListener (this);

    modeToggle->setBounds (420, 215, 96, 48);

    adsrGroup.reset (new GroupComponent ("adsrGroup",
                                         TRANS("Controls")));
    addAndMakeVisible (adsrGroup.get());
    adsrGroup->setTextLabelPosition (Justification::centredLeft);
    adsrGroup->setColour (GroupComponent::textColourId, Colours::aliceblue);

    adsrGroup->setBounds (365, 282, 280, 280);

    attackSlider.reset (new Slider ("attackSlider"));
    addAndMakeVisible (attackSlider.get());
    attackSlider->setTooltip (TRANS("Attack time (ms)"));
    attackSlider->setRange (0, 1000, 1);
    attackSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    attackSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    attackSlider->setColour (Slider::thumbColourId, Colour (0xff356931));
    attackSlider->setColour (Slider::trackColourId, Colour (0xff0a170c));
    attackSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff0a170c));
    attackSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0xff0a170c));
    attackSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x008e989b));
    attackSlider->addListener (this);

    decaySlider.reset (new Slider ("decaySlider"));
    addAndMakeVisible (decaySlider.get());
    decaySlider->setTooltip (TRANS("Decay time (ms)"));
    decaySlider->setRange (0, 1000, 1);
    decaySlider->setSliderStyle (Slider::RotaryVerticalDrag);
    decaySlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    decaySlider->setColour (Slider::thumbColourId, Colour (0xff356931));
    decaySlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0xff0a170c));
    decaySlider->setColour (Slider::textBoxOutlineColourId, Colour (0x008e989b));
    decaySlider->addListener (this);

    sustainSlider.reset (new Slider ("sustainSlider"));
    addAndMakeVisible (sustainSlider.get());
    sustainSlider->setTooltip (TRANS("Sustain level\n"));
    sustainSlider->setRange (0, 1, 0.01);
    sustainSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    sustainSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    sustainSlider->setColour (Slider::thumbColourId, Colour (0xff356931));
    sustainSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0xff0a170c));
    sustainSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x008e989b));
    sustainSlider->addListener (this);

    releaseSlider.reset (new Slider ("releaseSlider"));
    addAndMakeVisible (releaseSlider.get());
    releaseSlider->setTooltip (TRANS("Release time (ms)"));
    releaseSlider->setRange (0, 1000, 1);
    releaseSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    releaseSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    releaseSlider->setColour (Slider::thumbColourId, Colour (0xff356931));
    releaseSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0xff0a170c));
    releaseSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x008e989b));
    releaseSlider->addListener (this);

    dynamicVelocityToggle.reset (new ToggleButton ("dynamicVelocityToggle"));
    addAndMakeVisible (dynamicVelocityToggle.get());
    dynamicVelocityToggle->setTooltip (TRANS("Enable/disable dynamic velocity"));
    dynamicVelocityToggle->setButtonText (TRANS("Dynamic velocity"));
    dynamicVelocityToggle->addListener (this);
    dynamicVelocityToggle->setColour (ToggleButton::textColourId, Colours::aliceblue);

    dynamicVelocityToggle->setBounds (707, 288, 150, 24);

    adaptiveDecayToggle.reset (new ToggleButton ("adaptiveDecayToggle"));
    addAndMakeVisible (adaptiveDecayToggle.get());
    adaptiveDecayToggle->setTooltip (TRANS("Enable/disable adaptive decay"));
    adaptiveDecayToggle->setButtonText (TRANS("Adaptive decay"));
    adaptiveDecayToggle->addListener (this);
    adaptiveDecayToggle->setColour (ToggleButton::textColourId, Colours::aliceblue);

    pitchBendRangeSlider.reset (new Slider ("pitchBendRangeSlider"));
    addAndMakeVisible (pitchBendRangeSlider.get());
    pitchBendRangeSlider->setRange (0, 24, 1);
    pitchBendRangeSlider->setSliderStyle (Slider::LinearBar);
    pitchBendRangeSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    pitchBendRangeSlider->setColour (Slider::trackColourId, Colour (0x8a0a170c));
    pitchBendRangeSlider->setColour (Slider::textBoxTextColourId, Colours::aliceblue);
    pitchBendRangeSlider->addListener (this);

    pitchBendRangeLabel.reset (new Label ("pitchBendRangeLabel",
                                          TRANS("Pitch bend range (semitones)\n")));
    addAndMakeVisible (pitchBendRangeLabel.get());
    pitchBendRangeLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    pitchBendRangeLabel->setJustificationType (Justification::centredLeft);
    pitchBendRangeLabel->setEditable (false, false, false);
    pitchBendRangeLabel->setColour (Label::textColourId, Colours::aliceblue);
    pitchBendRangeLabel->setColour (TextEditor::textColourId, Colours::black);
    pitchBendRangeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    attackLabel.reset (new Label ("attackLabel",
                                  TRANS("A")));
    addAndMakeVisible (attackLabel.get());
    attackLabel->setTooltip (TRANS("Attack time"));
    attackLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
    attackLabel->setJustificationType (Justification::centredLeft);
    attackLabel->setEditable (false, false, false);
    attackLabel->setColour (Label::textColourId, Colour (0xe0f0f8ff));
    attackLabel->setColour (TextEditor::textColourId, Colours::black);
    attackLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    decayLabel.reset (new Label ("decayLabel",
                                 TRANS("D")));
    addAndMakeVisible (decayLabel.get());
    decayLabel->setTooltip (TRANS("Decay time"));
    decayLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
    decayLabel->setJustificationType (Justification::centredLeft);
    decayLabel->setEditable (false, false, false);
    decayLabel->setColour (Label::textColourId, Colour (0xe0f0f8ff));
    decayLabel->setColour (TextEditor::textColourId, Colours::black);
    decayLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    sustainLabel.reset (new Label ("sustainLabel",
                                   TRANS("S")));
    addAndMakeVisible (sustainLabel.get());
    sustainLabel->setTooltip (TRANS("Sustain level"));
    sustainLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
    sustainLabel->setJustificationType (Justification::centredLeft);
    sustainLabel->setEditable (false, false, false);
    sustainLabel->setColour (Label::textColourId, Colour (0xe0f0f8ff));
    sustainLabel->setColour (TextEditor::textColourId, Colours::black);
    sustainLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    releaseLabel.reset (new Label ("releaseLabel",
                                   TRANS("R")));
    addAndMakeVisible (releaseLabel.get());
    releaseLabel->setTooltip (TRANS("Release time"));
    releaseLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
    releaseLabel->setJustificationType (Justification::centredLeft);
    releaseLabel->setEditable (false, false, false);
    releaseLabel->setColour (Label::textColourId, Colour (0xe0f0f8ff));
    releaseLabel->setColour (TextEditor::textColourId, Colours::black);
    releaseLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (1024, 600);


    //[Constructor] You can add your own custom stuff here..
    //dynamicVelocityToggle->setColour(ToggleButton::ColourIds::tickColourId, Colours::black);
    //adaptiveDecayToggle->setColour(ToggleButton::ColourIds::tickColourId, Colours::black);

    valueTreeState.addParameterListener("mode", this);
    valueTreeState.addParameterListener("dynamicVelocity", this);
    valueTreeState.addParameterListener("adaptiveDecay", this);

    // Hook up sliders to state management
    positionAttachment.reset(new SliderAttachment(valueTreeState, "position", *positionSlider));
    windowLengthAttachment.reset(new SliderAttachment(valueTreeState, "windowLength", *windowLengthSlider));
    delayFeedbackAttachment.reset(new SliderAttachment(valueTreeState, "delayFeedback", *delayFeedbackSlider));
    modeAttachment.reset(new ButtonAttachment(valueTreeState, "mode", *modeToggle));
    attackAttachment.reset(new SliderAttachment(valueTreeState, "attack", *attackSlider));
    decayAttachment.reset(new SliderAttachment(valueTreeState, "decay", *decaySlider));
    sustainAttachment.reset(new SliderAttachment(valueTreeState, "sustain", *sustainSlider));
    releaseAttachment.reset(new SliderAttachment(valueTreeState, "release", *releaseSlider));
    dynamicVelocityAttachment.reset(new ButtonAttachment(valueTreeState, "dynamicVelocity", *dynamicVelocityToggle));
    adaptiveDecayAttachment.reset(new ButtonAttachment(valueTreeState, "adaptiveDecay", *adaptiveDecayToggle));
    pitchBendRangeAttachment.reset(new SliderAttachment(valueTreeState, "pitchBendRange", *pitchBendRangeSlider));

    // Set finer step size
    //double interval = 0.0001;
    //auto delayFeedbackRange = valueTreeState.getParameterRange("delayFeedback");
    //delayFeedbackSlider->setRange(delayFeedbackRange.getRange().getStart(), delayFeedbackRange.getRange().getEnd(), interval);

    // Enable/disable sliders based on mode
    bool isADSRMode = modeToggle->getToggleStateValue().getValue();
    delayFeedbackSlider->setEnabled(!isADSRMode);
    adaptiveDecayToggle->setEnabled(!isADSRMode);

    adsrGroup->setEnabled(isADSRMode);
    attackSlider->setEnabled(isADSRMode);
    decaySlider->setEnabled(isADSRMode);
    sustainSlider->setEnabled(isADSRMode);
    releaseSlider->setEnabled(isADSRMode);

    // Add child components
    addAndMakeVisible(samplePanel);

    // Disable mouse clicks for adsr labels
    attackLabel->setInterceptsMouseClicks(false, true);
    decayLabel->setInterceptsMouseClicks(false, true);
    sustainLabel->setInterceptsMouseClicks(false, true);
    releaseLabel->setInterceptsMouseClicks(false, true);

    // Set suffixes for adsr controls
    attackSlider->setTextValueSuffix(" ms");
    decaySlider->setTextValueSuffix(" ms");
    releaseSlider->setTextValueSuffix(" ms");
    //[/Constructor]
}

JUCEEditor::~JUCEEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //valueTreeState->removeParameterListener("mode", this);
    //valueTreeState->removeParameterListener("dynamicVelocity", this);
    //valueTreeState->removeParameterListener("adaptiveDecay", this);

    positionAttachment = nullptr;
    windowLengthAttachment = nullptr;
    delayFeedbackAttachment = nullptr;
    modeAttachment = nullptr;

    attackAttachment = nullptr;
    decayAttachment = nullptr;
    sustainAttachment = nullptr;
    releaseAttachment = nullptr;

    dynamicVelocityAttachment = nullptr;
    adaptiveDecayAttachment = nullptr;
    pitchBendRangeAttachment = nullptr;

    //[/Destructor_pre]

    positionSlider = nullptr;
    label = nullptr;
    windowLengthSlider = nullptr;
    label2 = nullptr;
    delayFeedbackSlider = nullptr;
    label3 = nullptr;
    modeToggle = nullptr;
    adsrGroup = nullptr;
    attackSlider = nullptr;
    decaySlider = nullptr;
    sustainSlider = nullptr;
    releaseSlider = nullptr;
    dynamicVelocityToggle = nullptr;
    adaptiveDecayToggle = nullptr;
    pitchBendRangeSlider = nullptr;
    pitchBendRangeLabel = nullptr;
    attackLabel = nullptr;
    decayLabel = nullptr;
    sustainLabel = nullptr;
    releaseLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void JUCEEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff446280));

    {
        int x = 0, y = 200, width = proportionOfWidth (0.3333f), height = 400;
        Colour fillColour = Colour (0xff4e1502);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = proportionOfWidth (0.3333f), y = 200, width = proportionOfWidth (0.3333f), height = 400;
        Colour fillColour = Colour (0xff941b0c);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 682, y = 200, width = proportionOfWidth (0.3400f), height = 402;
        Colour fillColour = Colour (0xffa33208);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 24, y = 224, width = 200, height = 30;
        String text (TRANS("Global controls"));
        Colour fillColour = Colours::aliceblue;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("Open Sans", 24.00f, Font::plain).withTypefaceStyle ("Light"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 365, y = 224, width = 50, height = 30;
        String text (TRANS("ADSR"));
        Colour fillColour = Colours::aliceblue;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("Open Sans", 24.00f, Font::plain).withTypefaceStyle ("Light"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 707, y = 224, width = 200, height = 30;
        String text (TRANS("Additional controls"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("Open Sans", 24.00f, Font::plain).withTypefaceStyle ("Light"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 0, y = 0, width = proportionOfWidth (1.0000f), height = 200;
        Colour fillColour = Colour (0xff2c3e50);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..

    // Draw shadow
    // Shadow for samplepanel
    auto shadow = DropShadow(Colours::black, 6, Point<int>(0, -3));
    int x = 0, y = 200, width = getWidth(), height = 1;
    Rectangle<int> area = Rectangle<int>(x, y, width, height);
    shadow.drawForRectangle(g, area);

    //[/UserPaint]
}

void JUCEEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    positionSlider->setBounds (24, 340 - (48 / 2), 280, 48);
    windowLengthSlider->setBounds (24, 433 - (48 / 2), 280, 48);
    delayFeedbackSlider->setBounds (24, 528 - (48 / 2), 280, 48);
    attackSlider->setBounds (365 + 29, 282 + 32, 100, 100);
    decaySlider->setBounds ((365 + 29) + 100 - -24, 282 + 32, 100, 100);
    sustainSlider->setBounds (365 + 29, 282 + 148, 100, 100);
    releaseSlider->setBounds ((365 + 29) + 100 - -24, 282 + 148, 100, 100);
    adaptiveDecayToggle->setBounds (707, 288 + 28, 150, 24);
    pitchBendRangeSlider->setBounds (707, ((288 + 28) + 28) + 28, 280, 48);
    pitchBendRangeLabel->setBounds (703, (288 + 28) + 28, 280, 24);
    attackLabel->setBounds ((365 + 29) + 100 / 2 + -10, (282 + 32) + 100 / 2 + -22, 20, 24);
    decayLabel->setBounds (((365 + 29) + 100 - -24) + 100 / 2 + -10, (282 + 32) + 100 / 2 + -22, 20, 24);
    sustainLabel->setBounds ((365 + 29) + 100 / 2 + -10, (282 + 148) + 100 / 2 + -22, 20, 24);
    releaseLabel->setBounds (((365 + 29) + 100 - -24) + 100 / 2 + -10, (282 + 148) + 100 / 2 + -22, 20, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void JUCEEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == positionSlider.get())
    {
        //[UserSliderCode_positionSlider] -- add your slider handling code here..
        //[/UserSliderCode_positionSlider]
    }
    else if (sliderThatWasMoved == windowLengthSlider.get())
    {
        //[UserSliderCode_windowLengthSlider] -- add your slider handling code here..
        //[/UserSliderCode_windowLengthSlider]
    }
    else if (sliderThatWasMoved == delayFeedbackSlider.get())
    {
        //[UserSliderCode_delayFeedbackSlider] -- add your slider handling code here..
        //[/UserSliderCode_delayFeedbackSlider]
    }
    else if (sliderThatWasMoved == attackSlider.get())
    {
        //[UserSliderCode_attackSlider] -- add your slider handling code here..
        //[/UserSliderCode_attackSlider]
    }
    else if (sliderThatWasMoved == decaySlider.get())
    {
        //[UserSliderCode_decaySlider] -- add your slider handling code here..
        //[/UserSliderCode_decaySlider]
    }
    else if (sliderThatWasMoved == sustainSlider.get())
    {
        //[UserSliderCode_sustainSlider] -- add your slider handling code here..
        //[/UserSliderCode_sustainSlider]
    }
    else if (sliderThatWasMoved == releaseSlider.get())
    {
        //[UserSliderCode_releaseSlider] -- add your slider handling code here..
        //[/UserSliderCode_releaseSlider]
    }
    else if (sliderThatWasMoved == pitchBendRangeSlider.get())
    {
        //[UserSliderCode_pitchBendRangeSlider] -- add your slider handling code here..
        //[/UserSliderCode_pitchBendRangeSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void JUCEEditor::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == modeToggle.get())
    {
        //[UserButtonCode_modeToggle] -- add your button handler code here..
        //[/UserButtonCode_modeToggle]
    }
    else if (buttonThatWasClicked == dynamicVelocityToggle.get())
    {
        //[UserButtonCode_dynamicVelocityToggle] -- add your button handler code here..
        //[/UserButtonCode_dynamicVelocityToggle]
    }
    else if (buttonThatWasClicked == adaptiveDecayToggle.get())
    {
        //[UserButtonCode_adaptiveDecayToggle] -- add your button handler code here..
        //[/UserButtonCode_adaptiveDecayToggle]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void JUCEEditor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == "mode") {
        if (newValue == 0.0f) {
            // String mode
            // Activate feedback slider
            delayFeedbackSlider->setEnabled(true);

            // Disable ADSR controls
            adsrGroup->setEnabled(false);
            attackSlider->setEnabled(false);
            decaySlider->setEnabled(false);
            sustainSlider->setEnabled(false);
            releaseSlider->setEnabled(false);

            // Enable option for adaptive decay
            adaptiveDecayToggle->setEnabled(true);
            repaint();
        }
        else {
            // Deactivate feedback slider in ADSR mode as feedback nees to be 1
            delayFeedbackSlider->setEnabled(false);

            // Enable ADSR controls
            attackSlider->setEnabled(true);
            decaySlider->setEnabled(true);
            sustainSlider->setEnabled(true);
            releaseSlider->setEnabled(true);
            adsrGroup->setEnabled(true);

            // Disable adaptive decay as it's not applicable in adsr mode
            adaptiveDecayToggle->setEnabled(false);
            repaint();
        }
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="JUCEEditor" componentName=""
                 parentClasses="public Component, private AudioProcessorValueTreeState::Listener"
                 constructorParams="Dafx_assignment_2AudioProcessor&amp; p, AudioProcessorValueTreeState&amp; vts, SamplePanel&amp; samplePanel"
                 variableInitialisers="processor(p), valueTreeState(vts), samplePanel(samplePanel)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="1024" initialHeight="600">
  <BACKGROUND backgroundColour="ff446280">
    <RECT pos="0 200 33.333% 400" fill="solid: ff4e1502" hasStroke="0"/>
    <RECT pos="33.333% 200 33.333% 400" fill="solid: ff941b0c" hasStroke="0"/>
    <RECT pos="682 200 34% 402" fill="solid: ffa33208" hasStroke="0"/>
    <TEXT pos="24 224 200 30" fill="solid: fff0f8ff" hasStroke="0" text="Global controls"
          fontname="Open Sans" fontsize="24.0" kerning="0.0" bold="0" italic="0"
          justification="33" typefaceStyle="Light"/>
    <TEXT pos="365.3 224 50 30" fill="solid: fff0f8ff" hasStroke="0" text="ADSR"
          fontname="Open Sans" fontsize="24.0" kerning="0.0" bold="0" italic="0"
          justification="33" typefaceStyle="Light"/>
    <TEXT pos="706.6 224 200 30" fill="solid: ffffffff" hasStroke="0" text="Additional controls"
          fontname="Open Sans" fontsize="24.0" kerning="0.0" bold="0" italic="0"
          justification="33" typefaceStyle="Light"/>
    <RECT pos="0 0 100% 200" fill="solid: ff2c3e50" hasStroke="0"/>
  </BACKGROUND>
  <SLIDER name="positionSlider" id="554566d500748e9" memberName="positionSlider"
          virtualName="" explicitFocusOrder="0" pos="24 340c 280 48" tooltip="Position in the loaded sample."
          trackcol="8a0a170c" textboxtext="fff0f8ff" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxAbove" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="new label" id="3221212aa8d2c3ca" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="20 288 150 24" textCol="fff0f8ff"
         edTextCol="ff000000" edBkgCol="0" labelText="Window position"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <SLIDER name="windowLengthSlider" id="4257b19175426af7" memberName="windowLengthSlider"
          virtualName="" explicitFocusOrder="0" pos="24 433c 280 48" tooltip="Position in the loaded sample."
          trackcol="8a0a170c" textboxtext="fff0f8ff" min="0.0" max="2400.0"
          int="1.0" style="LinearBar" textBoxPos="TextBoxAbove" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="new label" id="d2a8b098d2e15d0d" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="20 381 236 24" textCol="fff0f8ff"
         edTextCol="ff000000" edBkgCol="0" labelText="Window length (samples)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <SLIDER name="delayFeedbackSlider" id="8d84074f7c43be5a" memberName="delayFeedbackSlider"
          virtualName="" explicitFocusOrder="0" pos="24 528c 280 48" tooltip="Position in the loaded sample."
          trackcol="8a0a170c" textboxtext="fff0f8ff" min="0.0" max="1.0"
          int="0.0001" style="LinearBar" textBoxPos="TextBoxAbove" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="new label" id="245a3d6c800998a1" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="20 476 150 24" textCol="fff0f8ff"
         edTextCol="ff000000" edBkgCol="0" labelText="Delay feedback"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <TOGGLEBUTTON name="modeToggle" id="c70a6fab7ea27f59" memberName="modeToggle"
                virtualName="" explicitFocusOrder="0" pos="420 215 96 48" tooltip="Enable/disable ADSR mode"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <GROUPCOMPONENT name="adsrGroup" id="df54a6fd7be47f34" memberName="adsrGroup"
                  virtualName="" explicitFocusOrder="0" pos="365 282 280 280" textcol="fff0f8ff"
                  title="Controls" textpos="33"/>
  <SLIDER name="attackSlider" id="e43f7da2edeaeb5f" memberName="attackSlider"
          virtualName="" explicitFocusOrder="0" pos="29 32 100 100" posRelativeX="df54a6fd7be47f34"
          posRelativeY="df54a6fd7be47f34" tooltip="Attack time (ms)" thumbcol="ff356931"
          trackcol="ff0a170c" rotarysliderfill="ff0a170c" rotaryslideroutline="ff0a170c"
          textboxoutline="8e989b" min="0.0" max="1000.0" int="1.0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="decaySlider" id="500662f4fe428399" memberName="decaySlider"
          virtualName="" explicitFocusOrder="0" pos="-24R 32 100 100" posRelativeX="e43f7da2edeaeb5f"
          posRelativeY="df54a6fd7be47f34" tooltip="Decay time (ms)" thumbcol="ff356931"
          rotaryslideroutline="ff0a170c" textboxoutline="8e989b" min="0.0"
          max="1000.0" int="1.0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="sustainSlider" id="6a316a9088d1b20f" memberName="sustainSlider"
          virtualName="" explicitFocusOrder="0" pos="29 148 100 100" posRelativeX="df54a6fd7be47f34"
          posRelativeY="df54a6fd7be47f34" tooltip="Sustain level&#10;"
          thumbcol="ff356931" rotaryslideroutline="ff0a170c" textboxoutline="8e989b"
          min="0.0" max="1.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="releaseSlider" id="b5890c334c04ffe5" memberName="releaseSlider"
          virtualName="" explicitFocusOrder="0" pos="-24R 148 100 100"
          posRelativeX="e43f7da2edeaeb5f" posRelativeY="df54a6fd7be47f34"
          tooltip="Release time (ms)" thumbcol="ff356931" rotaryslideroutline="ff0a170c"
          textboxoutline="8e989b" min="0.0" max="1000.0" int="1.0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <TOGGLEBUTTON name="dynamicVelocityToggle" id="7a2154e14461c851" memberName="dynamicVelocityToggle"
                virtualName="" explicitFocusOrder="0" pos="707 288 150 24" tooltip="Enable/disable dynamic velocity"
                txtcol="fff0f8ff" buttonText="Dynamic velocity" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="adaptiveDecayToggle" id="859efb8c26bba33b" memberName="adaptiveDecayToggle"
                virtualName="" explicitFocusOrder="0" pos="707 28 150 24" posRelativeY="7a2154e14461c851"
                tooltip="Enable/disable adaptive decay" txtcol="fff0f8ff" buttonText="Adaptive decay"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="pitchBendRangeSlider" id="2cee9c329bab7ceb" memberName="pitchBendRangeSlider"
          virtualName="" explicitFocusOrder="0" pos="707 28 280 48" posRelativeY="15d5ac52981382a0"
          trackcol="8a0a170c" textboxtext="fff0f8ff" min="0.0" max="24.0"
          int="1.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="pitchBendRangeLabel" id="15d5ac52981382a0" memberName="pitchBendRangeLabel"
         virtualName="" explicitFocusOrder="0" pos="703 28 280 24" posRelativeY="859efb8c26bba33b"
         textCol="fff0f8ff" edTextCol="ff000000" edBkgCol="0" labelText="Pitch bend range (semitones)&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <LABEL name="attackLabel" id="1b9d91694d079ed9" memberName="attackLabel"
         virtualName="" explicitFocusOrder="0" pos="-10C -22C 20 24" posRelativeX="e43f7da2edeaeb5f"
         posRelativeY="e43f7da2edeaeb5f" tooltip="Attack time" textCol="e0f0f8ff"
         edTextCol="ff000000" edBkgCol="0" labelText="A" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <LABEL name="decayLabel" id="96a95b6be0dcd43c" memberName="decayLabel"
         virtualName="" explicitFocusOrder="0" pos="-10C -22C 20 24" posRelativeX="500662f4fe428399"
         posRelativeY="500662f4fe428399" tooltip="Decay time" textCol="e0f0f8ff"
         edTextCol="ff000000" edBkgCol="0" labelText="D" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <LABEL name="sustainLabel" id="567fd5641510f647" memberName="sustainLabel"
         virtualName="" explicitFocusOrder="0" pos="-10C -22C 20 24" posRelativeX="6a316a9088d1b20f"
         posRelativeY="6a316a9088d1b20f" tooltip="Sustain level" textCol="e0f0f8ff"
         edTextCol="ff000000" edBkgCol="0" labelText="S" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <LABEL name="releaseLabel" id="779652ad0222704e" memberName="releaseLabel"
         virtualName="" explicitFocusOrder="0" pos="-10C -22C 20 24" posRelativeX="b5890c334c04ffe5"
         posRelativeY="b5890c334c04ffe5" tooltip="Release time" textCol="e0f0f8ff"
         edTextCol="ff000000" edBkgCol="0" labelText="R" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]


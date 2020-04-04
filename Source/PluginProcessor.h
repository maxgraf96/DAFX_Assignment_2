/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SamplePanel.h"
#include "Delay.h"
#include "dywapitchtrack-master/src/dywapitchtrack.h"
#include "Voice.h"

//==============================================================================
/**
*/
class Dafx_assignment_2AudioProcessor : public AudioProcessor, private AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    Dafx_assignment_2AudioProcessor();
    ~Dafx_assignment_2AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    SamplePanel* getSamplePanel();

    // Delay methods
    void setDelayFeedback(float delayFeedback);
    void setDelayWet(float delayWet);

private:
    // State management
    AudioProcessorValueTreeState parameters;
    void parameterChanged(const String& parameterID, float newValue) override;

    // Whether there should be any audio coming through
    bool isPlaying = false;
    bool isCapturing = false;

    // Audio buffer holding sample
    AudioBuffer<float> *sampleBuffer = nullptr;

    // Window size (length)
    int windowLength = WINDOW_LENGTH_MIN; 

    // SamplePanel component
    std::unique_ptr<SamplePanel> samplePanel;
    int samplePanelStartIdx = 0;

    // Control parameters
    // Controls the sample position in seconds
    float* positionParam = nullptr;
    float* windowLengthParam = nullptr;
    float* delayFeedbackParam = nullptr;
    float* delayWetParam = nullptr;

    // Polyphony
    const int NUM_VOICES = 16;
    std::vector<std::unique_ptr<Voice>> voices = {};

    // Pitch detection
    // dywapitchtracker pitchtracker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessor)
};

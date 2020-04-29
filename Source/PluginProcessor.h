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
#include "Voice.h"
#include "Constants.h"

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

    // Delay methods
    void setDelayFeedback(float delayFeedback);

    // Main lowpass filter getter
    std::array<juce::dsp::IIR::Filter<float>, 2>& getMainLowpassFilters();

    void updateMainLowpassFilters(float cutoff, float q);
    AudioProcessorValueTreeState& getVTS();

private:
    // State management
    AudioProcessorValueTreeState parameters;
    void parameterChanged(const String& parameterID, float newValue) override;

    // Audio buffer holding sample
    AudioBuffer<float> *sampleBuffer = nullptr;

    // Window size (length)
    int windowLength = WINDOW_LENGTH_MIN;
    int prevWindowLength = -1;

    // SamplePanel component
    std::unique_ptr<SamplePanel> samplePanel;
    int samplePanelStartIdx = 0;
    int prevSamplePanelStartIdx = 0;

    // Control parameters
    // Controls the sample position in seconds
    float* positionParam = nullptr;
    float* windowLengthParam = nullptr;
    float* delayFeedbackParam = nullptr;
    float* modeParam = nullptr;
    float* attackParam = nullptr;
    float* decayParam = nullptr;
    float* sustainParam = nullptr;
    float* releaseParam = nullptr;
    // Whether to use the same velocity for every stroke
    float* dynamicVelocityParam = nullptr;
    float* adaptiveDecayParam = nullptr;
    // Pitch bend range
    float* pitchBendRangeParam = nullptr;
    // Main lowpass filter cutoff frequency and Q value
    float* mainFilterCutoffParam = nullptr;
    float* mainFilterQParam = nullptr;
    float* mainOutputGainParam = nullptr;

    // ADSR parameters
    ADSR::Parameters adsrParams;

    // Polyphony
    std::vector<std::unique_ptr<Voice>> voices = {};
    // Map from note voices to notenumber
    // So: { -1 -1 74 -1 -1 ... } means that the third voice is currently playing MIDI note 74
    std::array<int, NUM_VOICES> noteNumberForVoice = {};

    // Reset flag for voices
    // Some voice parameters can't be changed during playback, so this guard will tell the 
    // main loop not to use the voices while changes are being made
    bool shouldVoicesChange = false;
    // Callback for when voices should change
    // This includes switching from normal mode to ADSR, and activating/deactivating adaptive decay times
    void changeVoices();

    // Mapped pitch wheel control [0...1]
    float mappedPitchWheelValue = 0.0f;
    // How far to shift up/down
    int pitchBendRange = 12;

    // Main lowpass filter 
    std::array<juce::dsp::IIR::Filter<float>, 2> mainLowpassFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessor)
};

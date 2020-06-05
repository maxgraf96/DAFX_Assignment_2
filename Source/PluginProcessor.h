/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SamplePanel.h"
#include "Delay.h"
#include "Voice.h"
#include "Constants.h"

using namespace juce;

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
	// Load plugin state information from XML
    void getStateInformation (MemoryBlock& destData) override;
	// Save plugin state information to XML
    void setStateInformation (const void* data, int sizeInBytes) override;

    /*
     *Setter for delay feedback
     *This applies the incoming value to all voices
	*/
    void setDelayFeedback(float delayFeedback);

    // Main lowpass filter getter
    std::array<dsp::IIR::Filter<float>, 2>& getMainLowpassFilters();

	// Update the cutoff frequency / q value for the main lowpass filters
    void updateMainLowpassFilters(float cutoff, float q);

    AudioProcessorValueTreeState& getVTS();

private:
    // State management
    AudioProcessorValueTreeState parameters;
	// Called if one of the parameters is changed, either through UI interaction or
	// manipulation from the host (such as automations)
    void parameterChanged(const String& parameterID, float newValue) override;

    // Audio buffer holding the full loaded sample
    AudioBuffer<float> *sampleBuffer = nullptr;

    // Length of the window used to extract the excitator burst from the full sample
    int windowLength = WINDOW_LENGTH_MIN;
	// Placeholder to check if the window length changed
    int prevWindowLength = -1;

    // SamplePanel component
    std::unique_ptr<SamplePanel> samplePanel;
	// The lower end of the window used to extract the excitator burst
    int samplePanelStartIdx = 0;
    // Placeholder to check if the window position changed
    int prevSamplePanelStartIdx = 0;

    // Control parameters tied to the audio parameters
	// Due to the JUCE handling these are all converted to float values
	// Current position in sample (centre of window)
    std::atomic<float>* positionParam = nullptr;
	// Current window length
    std::atomic<float>* windowLengthParam = nullptr;
	// Current delay feedback value [0...1]
    std::atomic<float>* delayFeedbackParam = nullptr;
	// Whether ADSR mode is enabled/disabled
    std::atomic<float>* adsrModeParam = nullptr;
	// Attack time (ms)
    std::atomic<float>* attackParam = nullptr;
	// Decay time (ms)
    std::atomic<float>* decayParam = nullptr;
	// Sustain level [0...1]
    std::atomic<float>* sustainParam = nullptr;
	// Release time (ms)
    std::atomic<float>* releaseParam = nullptr;
    // Whether to use a fixed velocity (127 =^= gain of 1.0) for every incoming note
	// Or use the velocity coming from the MIDI data
    std::atomic<float>* dynamicVelocityParam = nullptr;
	// Whether to equalise decay times of notes across different pitches
	// Normally, due to the synthesis paradigm, higher pitched notes decay faster
	// If this is enabled, this difference in decays is corrected by adapting the current delay feedback of the voice
	// to the fundamental frequency of the incoming note
    std::atomic<float>* adaptiveDecayParam = nullptr;
    // Pitch bend range
    std::atomic<float>* pitchBendRangeParam = nullptr;
    // Main lowpass filter cutoff frequency and Q values
    std::atomic<float>* mainFilterCutoffParam = nullptr;
    std::atomic<float>* mainFilterQParam = nullptr;
	// Main output gain
    std::atomic<float>* mainOutputGainParam = nullptr;

    // ADSR parameters
    ADSR::Parameters adsrParams;

    // Polyphony
	// Vector that will be filled with NUM_VOICES voices (defined in Constants.h)
    std::vector<std::unique_ptr<Voice>> voices = {};
    // Map from voice index to MIDI note number, used to keep track of which voices are taken and which voices are free
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

    // Main lowpass filters (one per channel, stereo is assumed at the moment) 
    std::array<dsp::IIR::Filter<float>, 2> mainLowpassFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessor)
};

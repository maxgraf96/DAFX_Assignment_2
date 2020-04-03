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

//==============================================================================
/**
*/
class Dafx_assignment_2AudioProcessor : public AudioProcessor
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
    void setDelayTime(int delayTimeSamples);
    void setDelayFeedback(float delayFeedback);
    void setDelayWet(float delayWet);

    void playNote(int noteNumber);

private:
    // Whether there should be any audio coming through
    bool isPlaying = false;
    bool isCapturing = false;

    // Audio buffer holding sample
    AudioBuffer<float> *sampleBuffer = nullptr;

    // Window size (length)
    int windowLength = WINDOW_LENGTH_MIN; 
    // Current PLAYING position (in terms from window start to end)
    // This is used to transfer data from the global sample buffer to the plugin buffer
    int playingPosition = 0;

    // SamplePanel component
    std::unique_ptr<SamplePanel> samplePanel;
    int samplePanelStartIdx = 0;

    // Control parameters
    // Controls the sample position in seconds
    AudioParameterFloat* windowLengthParam;
    AudioParameterInt* delayTimeParam;
    AudioParameterFloat* delayFeedbackParam;
    AudioParameterFloat* delayWetParam;

    // This buffer stores a subset of the sample buffer for playback
    std::unique_ptr<AudioBuffer<float>> playbackBuffer;

    // Delay object
    std::unique_ptr<Delay> delay;

    // Pitch detection
    dywapitchtracker pitchtracker;
    // Used to calculate avg frequency over n_frames
    // int n_frames = 4;
    // This stores the detected pitch of the sound in the capture buffer
    // It is defined by the width of the window
    // double currentPitch = 0.0;
    // Index storing where the buffer should end to get the desired frequency
    // Set in playNote()
    bool playFromPlaybackBuffer = false;
    int playbackBufferSamplePosition = 0;

    // MIDI stuff
    double midiStartTime = 0.0;
    const int midiChannel = 0;
    MidiBuffer midiBuffer;
    int previousSampleNumber = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessor)
};

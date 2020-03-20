/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SamplePanel.h"

//==============================================================================
/**
*/
class Dafx_assignment_2AudioProcessor  : public AudioProcessor
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

private:

    // Audio buffer holding sample
    AudioBuffer<float> *sampleBuffer = nullptr;

    // Window size (length)
    int windowSize = 4800; // 100ms if sample rate is 48kHz
    // Current PLAYING position (in terms from window start to end)
    // This is used to transfer data from the global sample buffer to the plugin buffer
    int playingPosition = 0;

    // SamplePanel component
    std::unique_ptr<SamplePanel> samplePanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_2AudioProcessor)
};

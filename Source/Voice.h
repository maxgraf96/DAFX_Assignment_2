/*
  ==============================================================================

    Voice.h
    Created: 4 Apr 2020 1:36:09pm
    Author:  Music

    One voice for the synthesis process. Several of these are combined into a vector
    in order to achieve polyphony.
  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Delay.h"
#include "Constants.h"
#include "Utility.h"

class Voice
{
public:
    Voice(AudioBuffer<float>* sampleBuffer, juce::dsp::ProcessSpec delayProcessContext, int bufferLength, std::array<int, NUM_VOICES>& noteNumberForVoice);
    // To trigger a voice
    void noteOn(int noteNumber, uint8 velocity, int samplePanelStartIdx, int windowLength);
    void noteOff();
    // To copy samples from voice buffer to main audio buffer
    void play(AudioBuffer<float>& mainBuffer, int samplePanelStartIdx, int windowLength, bool windowChanged, float pitchWheelValue);
    void endNoteInMap();
    void setDelayFeedback(float delayFeedback);
    void setDelayWet(float delayWet);
    bool isPlaying();
    int getNoteNumber();
    static const int NOT_PLAYING = -1;
    void setADSRParams(ADSR::Parameters& params);
    void setADSRMode(bool mode);
	void setAdaptiveDecay(bool on);
    void setPitchBendRange(int pitchBendRange);
    void resetVoice();

private:
    // Normal (false) vs. ADSR (true)
    bool adsrMode = false;
    bool playing = false;
    int noteNumber = -1;
    int sampleRate = 0;
    Delay delay;
    // The buffer storing the generated sound
    std::unique_ptr<AudioBuffer<float>> buffer;
    // Extra buffer for processing
    std::unique_ptr<AudioBuffer<float>> processBuffer;

    // Reference to the buffer storing the sample
    // Same for all voices
    AudioBuffer<float>& sampleBuffer;
    
    // Current position in buffer
    int bufferPosition = NOT_PLAYING; // -1 means not playing

    // Volume envelope
    ADSR adsr;

    // Reference to note number to voices map 
    // This is necessary to have here because if the voice is in ADSR mode
    // it needs to wait until the release phase is over before releasing the slot
    std::array<int, NUM_VOICES>& noteNumberForVoice;

    float currentVelocity = 0.0f;
    double currentFrequency = 0.0f;

    // Pitch bend range in semitones
    int pitchBendRange = 0;
};
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
#include "Delay.h"
#include "Constants.h"
#include "Utility.h"

class Voice
{
public:
	// Constructor given a reference to the main sample buffer,
	// a process context, the buffer length and the map of note-numbers to voices
    Voice(AudioBuffer<float>& sampleBuffer, dsp::ProcessSpec processContext, int bufferLength, std::array<int, NUM_VOICES>& noteNumberForVoice);
    // To trigger a voice
    void noteOn(int noteNumber, uint8 velocity, int samplePanelStartIdx, int windowLength);
    // To end a note
	// In ADSR mode this triggers the release phase
	// In normal mode this does nothing
	void noteOff();
    // To copy samples from voice buffer to main audio buffer
    void play(AudioBuffer<float>& mainBuffer, int samplePanelStartIdx, int windowLength, bool windowChanged, float pitchWheelValue);
    // To set the assigned voice slot in the map to NOT_PLAYING
	void endNoteInMap();
    void setDelayFeedback(float delayFeedback);
    void setDelayWet(float delayWet);
	// Set ADSR parameters
    void setADSRParams(ADSR::Parameters& params);
	// Set the mode (ADSR on/off)
    void setADSRMode(bool mode);
	// Set the adaptive decay mode
	void setAdaptiveDecay(bool on);
	// Set the pitch bend range
    void setPitchBendRange(int pitchBendRange);
	// Reset voice to not playing
    void resetVoice();

private:
    // Normal (false) vs. ADSR (true)
    bool adsrMode = false;
	// The current note number
    int noteNumber = NOT_PLAYING;
	// The delay object used to excite the string
    Delay delay;
    // The buffer storing the windowed section of the main sample
    std::unique_ptr<AudioBuffer<float>> buffer;
    // Extra buffer for processing
    std::unique_ptr<AudioBuffer<float>> processBuffer;

    // Reference to the buffer storing the sample
    // Same for all voices
    AudioBuffer<float>& sampleBuffer;
    
    // Current position in buffer
    int bufferPosition = NOT_PLAYING;

    // Volume envelope
    ADSR adsr;

    // Reference to note number to voices map 
    // This is necessary to have here because if the voice is in ADSR mode
    // it needs to wait until the release phase is over before releasing its slot
    std::array<int, NUM_VOICES>& noteNumberForVoice;

	// Current velocity of note
    float currentVelocity = 0.0f;
	// Current fundamental frequency (coming from incoming MIDI data)
    double currentFrequency = 0.0f;

    // Pitch bend range in semitones
    int pitchBendRange = 0;
};
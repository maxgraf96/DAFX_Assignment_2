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
class Voice
{
public:
    Voice(AudioBuffer<float>* sampleBuffer, juce::dsp::ProcessSpec delayProcessContext, int bufferLength);
    // To trigger a voice
    void noteOn(int noteNumber, int samplePanelStartIdx, int windowLength);
    void noteOff();
    // To copy samples from voice buffer to main audio buffer
    void play(AudioBuffer<float>& mainBuffer);
    void setDelayFeedback(float delayFeedback);
    void setDelayWet(float delayWet);
    bool isPlaying();
    int getNoteNumber();
    static const int NOT_PLAYING = -1;
    void setADSRParams(ADSR::Parameters& params);
    void setMode(bool mode);

private:
    // Normal (false) vs. ADSR (true)
    bool mode = false;
    bool playing = false;
    int noteNumber = -1;
    int sampleRate = 0;
    Delay delay;
    // The buffer storing the generated sound
    std::unique_ptr<AudioBuffer<float>> buffer;

    // Pointer to the buffer storing the sample
    // Same for all voices
    AudioBuffer<float>& sampleBuffer;
    
    // Current position in buffer
    int bufferPosition = NOT_PLAYING; // -1 means not playing

    // Volume envelope
    ADSR adsr;
};
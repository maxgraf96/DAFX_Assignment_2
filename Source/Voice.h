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
    // To copy samples from voice buffer to main audio buffer
    void play(AudioBuffer<float>& mainBuffer);
    void setDelayFeedback(float delayFeedback);
    void setDelayWet(float delayWet);
    bool isPlaying();
    static const int NOT_PLAYING = -1;

private:
    int sampleRate = 0;
    Delay delay;
    // The buffer storing the generated sound
    std::unique_ptr<AudioBuffer<float>> buffer;

    // Pointer to the buffer storing the sample
    // Same for all voices
    AudioBuffer<float>& sampleBuffer;
    
    // Current position in buffer
    int bufferPosition = NOT_PLAYING; // -1 means not playing
};
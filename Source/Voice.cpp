/*
  ==============================================================================

    Voice.cpp
    Created: 4 Apr 2020 1:36:09pm
    Author:  Music

  ==============================================================================
*/

#include "Voice.h"
Voice::Voice(AudioBuffer<float>* sampleBuffer, juce::dsp::ProcessSpec delayProcessContext, int bufferLength)
:sampleBuffer(*sampleBuffer) {
    sampleRate = delayProcessContext.sampleRate;
    // Setup delay object
    delay.setFeedback(1.0f);
    delay.setWetLevel(1.0f);

    // Setup buffer
    buffer.reset(new AudioBuffer<float>(2, bufferLength));
}

void Voice::noteOn(int noteNumber, int samplePanelStartIdx, int windowLength)
{
    // Reset capture buffer sample position
    bufferPosition = 0;

    // Reset playback buffer
    buffer->clear();

    // Convert note number to frequency
    auto frequency = MidiMessage::getMidiNoteInHertz(noteNumber);

    // Convert frequency to delay time (seconds)
    double delayTime = 1.0 / frequency;

    // Convert delay time seconds to samples
    int delayTimeSamples = round(delayTime * sampleRate);

    // Set delay time for both channels
    delay.setDelayTime(0, delayTimeSamples);
    delay.setDelayTime(1, delayTimeSamples);

    // Load window into playback buffer
    for (int channel = 0; channel < sampleBuffer.getNumChannels(); channel++)
        buffer->copyFrom(channel, 0, sampleBuffer, channel, samplePanelStartIdx, windowLength);
}

void Voice::play(AudioBuffer<float>& mainBuffer)
{
    // Get number of samples to copy
    auto numSamplesToCopy = mainBuffer.getNumSamples();
    auto finished = false;
    if (bufferPosition + numSamplesToCopy >= buffer->getNumSamples()) {
        // If we are at the end of the capture buffer
        numSamplesToCopy = buffer->getNumSamples() - bufferPosition;
        // Stop playback for next block
        finished = true;
    }

    // Create subbuffer
    auto* subbuffer = new AudioBuffer<float>(buffer->getNumChannels(), numSamplesToCopy);
    for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
        subbuffer->copyFrom(channel, 0, *buffer, channel, bufferPosition, numSamplesToCopy);
    }
    // Feed signal into delay line
    delay.process(*subbuffer);
    subbuffer->applyGain(0.5f);

    for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
        mainBuffer.addFrom(channel, 0, *subbuffer, channel, 0, numSamplesToCopy);
    }

    // Update sample counter
    bufferPosition += numSamplesToCopy;

    // Quit if finished
    if (finished) {
        bufferPosition = NOT_PLAYING;
    }

    delete subbuffer;
}

void Voice::setDelayFeedback(float delayFeedback)
{ 
    delay.setFeedback(delayFeedback);
}

void Voice::setDelayWet(float delayWet)
{
    delay.setWetLevel(delayWet);
}

bool Voice::isPlaying()
{
    return bufferPosition != NOT_PLAYING;
}

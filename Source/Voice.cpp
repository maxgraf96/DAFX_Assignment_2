/*
  ==============================================================================

    Voice.cpp
    Created: 4 Apr 2020 1:36:09pm
    Author:  Music

  ==============================================================================
*/

#include "Voice.h"
Voice::Voice(AudioBuffer<float>* sampleBuffer, juce::dsp::ProcessSpec delayProcessContext, int bufferLength, std::array<int, NUM_VOICES>& noteNumberForVoice)
:sampleBuffer(*sampleBuffer), noteNumberForVoice(noteNumberForVoice) {
    sampleRate = delayProcessContext.sampleRate;
    // Setup delay object
    delay.prepare(delayProcessContext);
    delay.setFeedback(1.0f);
    delay.setWetLevel(1.0f);

    // Set adsr sample rate
    adsr.setSampleRate(delayProcessContext.sampleRate);

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

    // Prepare delay for fine-tuning (also sets delay length)
    delay.prepareFineTune(frequency);

    // Load window into playback buffer
    for (int channel = 0; channel < sampleBuffer.getNumChannels(); channel++)
        buffer->copyFrom(channel, 0, sampleBuffer, channel, samplePanelStartIdx, windowLength);

    // Set note number
    this->noteNumber = noteNumber;
    // Start envelope
    if(adsrMode)
        adsr.noteOn();
    playing = true;
}

void Voice::noteOff()
{
    if(adsrMode)
        adsr.noteOff();
}

void Voice::play(AudioBuffer<float>& mainBuffer)
{
    // Always check if adsr is done, if so, reset buffer position
    if (adsrMode && !adsr.isActive()) {
        // Find this voice's number in notes and reset
        for (int& nn : noteNumberForVoice) {
            if (nn == noteNumber) {
                nn = NOT_PLAYING;
                break;
            }
        }
        // Reset note number
        noteNumber = NOT_PLAYING;
        playing = false;

    }
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
    processBuffer.reset(new AudioBuffer<float>(buffer->getNumChannels(), numSamplesToCopy));
    for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
        processBuffer->copyFrom(channel, 0, *buffer, channel, bufferPosition, numSamplesToCopy);
    }

    // Feed signal into delay line
    delay.process(*processBuffer);

    // Apply volume envelope if in synth mode
    if (adsrMode) {
        adsr.applyEnvelopeToBuffer(*processBuffer, 0, numSamplesToCopy);
    }

    // Copy audio data into main buffer
    for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
        // If finished apply gain ramp
        mainBuffer.addFrom(channel, 0, *processBuffer, channel, 0, numSamplesToCopy);
    }

    // Update sample counter
    bufferPosition += numSamplesToCopy;

    // What to do when end of buffer reached:
    // If mode is normal, stop playing
    // If mode is ADSR, just reset the buffer position to 0 (loop the buffer as volume is controlled by envelope)
    if (finished) {
        if (!adsrMode) {
            // Normal mode
            playing = false;
            bufferPosition = NOT_PLAYING;
        }
        else {
            // ADSR mode -> reset buffer position
            // Note: The reset here goes to a little over 0 to avoid the initial click
            bufferPosition = int(buffer->getNumSamples() / 3);
        }
    }
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
    return playing;
}

int Voice::getNoteNumber()
{
    return noteNumber;
}

void Voice::setADSRParams(ADSR::Parameters& params)
{
    adsr.setParameters(params);
}

void Voice::setADSRMode(bool mode)
{
    this->adsrMode = mode;
}

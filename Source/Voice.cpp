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
    processBuffer.reset(new AudioBuffer<float>(2, delayProcessContext.maximumBlockSize));
}

void Voice::noteOn(int noteNumber, uint8 velocity, int samplePanelStartIdx, int windowLength)
{
    // Reset capture buffer sample position
    bufferPosition = 0;

    // Reset playback buffer
    buffer->clear();

    // Convert note number to frequency
    currentFrequency = MidiMessage::getMidiNoteInHertz(noteNumber);

    // Reset delay elements (filters and delay line)
    delay.reset();

    // Prepare delay for fine-tuning (also sets delay length)
    delay.prepareFineTune(currentFrequency, 0.0f);

    // Load window into playback buffer
    for (int channel = 0; channel < sampleBuffer.getNumChannels(); channel++)
        buffer->copyFrom(channel, 0, sampleBuffer, channel, samplePanelStartIdx, windowLength);

    // Attenuate according to velocity
    currentVelocity = map(float(velocity), 0.0, 127.0, 0.0, 1.0);
    buffer->applyGain(currentVelocity);

    // Set note number
    this->noteNumber = noteNumber;
    // Start envelope
    if(adsrMode)
        adsr.noteOn();
    playing = true;
}

void Voice::noteOff()
{
    if (adsrMode)
        adsr.noteOff();
}

void Voice::play(AudioBuffer<float>& mainBuffer, int samplePanelStartIdx, int windowLength, bool windowChanged, float pitchWheelValue)
{
    // Always check if adsr is done, if so, reset buffer position
    if (adsrMode && !adsr.isActive()) {
        resetVoice();
        return;
    }

    // Update the playback buffer if the window changed
    if (windowChanged) {
        // Reset playback buffer
        buffer->clear();
        // Load window into playback buffer
        for (int channel = 0; channel < sampleBuffer.getNumChannels(); channel++)
            buffer->copyFrom(channel, 0, sampleBuffer, channel, samplePanelStartIdx, windowLength);

        delay.windowChanged();
        delay.prepareFineTune(currentFrequency, pitchWheelValue);
        bufferPosition = int(windowLength / 2);
    }

    delay.prepareFineTune(currentFrequency, pitchWheelValue);

    // Get number of samples to copy - this corresponds to the block size specified by the plugin host
    auto numSamplesToCopy = mainBuffer.getNumSamples();
    auto isBufferEndReached = false;
    if (bufferPosition + numSamplesToCopy >= buffer->getNumSamples()) {
        // If we are at the end of the capture buffer
        numSamplesToCopy = buffer->getNumSamples() - bufferPosition;
        // Stop playback for next block if in non-pad mode
        isBufferEndReached = true;
    }

    // Create subbuffer
    for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
        processBuffer->copyFrom(channel, 0, *buffer, channel, bufferPosition, numSamplesToCopy);
    }

    // Feed signal into delay line
    delay.process(*processBuffer);

    // Apply gain for velocity
    processBuffer->applyGain(currentVelocity);

    // Apply volume envelope if in synth mode
    if (adsrMode) {
        adsr.applyEnvelopeToBuffer(*processBuffer, 0, numSamplesToCopy);
    }

    // Copy audio data into main buffer
    for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
        mainBuffer.addFrom(channel, 0, *processBuffer, channel, 0, numSamplesToCopy);
    }

    // Update sample counter
    bufferPosition += numSamplesToCopy;

    // What to do when end of buffer reached:
    // If mode is normal, stop playing
    // If mode is ADSR, just reset the buffer position to 0 (loop the buffer as volume is controlled by envelope)
    if (isBufferEndReached) {
        if (!adsrMode) {
            // Normal mode
            resetVoice();
        }
        else {
            // ADSR mode -> reset buffer position
            // Note: The reset here goes to the half point of the buffer to avoid the initial pluck sound when padding
            bufferPosition = int(buffer->getNumSamples() / 2);
        }
    }
}

void Voice::resetVoice()
{
    endNoteInMap();
    bufferPosition = NOT_PLAYING;
    buffer->clear();
    processBuffer->clear();
    playing = false;
}

void Voice::endNoteInMap() {
    for (int& nn : noteNumberForVoice) {
        if (nn == noteNumber) {
            nn = NOT_PLAYING;
            return;
        }
    }
    noteNumber = NOT_PLAYING;
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

void Voice::setAdaptiveDecay(bool isAdaptiveDecay) {
    delay.setAdaptiveDecay(isAdaptiveDecay);
}

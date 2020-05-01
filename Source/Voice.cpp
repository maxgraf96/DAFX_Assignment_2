/*
  ==============================================================================

    Voice.cpp
    Created: 4 Apr 2020 1:36:09pm
    Author:  Music

  ==============================================================================
*/

#include "Voice.h"
Voice::Voice(AudioBuffer<float>& sampleBuffer, dsp::ProcessSpec processContext, int bufferLength, std::array<int, NUM_VOICES>& noteNumberForVoice)
:sampleBuffer(sampleBuffer), noteNumberForVoice(noteNumberForVoice) {
    // Setup delay object
    delay.prepare(processContext);
    delay.setFeedback(1.0f);
    delay.setWetLevel(1.0f);

    // Set adsr sample rate
    adsr.setSampleRate(processContext.sampleRate);

    // Setup buffers (stereo sound is assumed at the moment)
    buffer.reset(new AudioBuffer<float>(2, bufferLength));
    processBuffer.reset(new AudioBuffer<float>(2, processContext.maximumBlockSize));
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

    // Prepare delay for fine-tuning (also sets the delay-line length)
    delay.prepareFineTune(currentFrequency, pitchBendRange, 0.0f);

	// Check if window length is out of range
	const auto numSamplesInBuffer = sampleBuffer.getNumSamples();
    auto numSamplesToCopy = windowLength;
	if(samplePanelStartIdx + numSamplesToCopy > numSamplesInBuffer)
        numSamplesToCopy = sampleBuffer.getNumSamples() - numSamplesToCopy;
	// If the window is made very large in a small file then use the end of the sample buffer instead
    if(numSamplesToCopy < 0)
        numSamplesToCopy = (WINDOW_LENGTH_MAX > numSamplesInBuffer ? numSamplesInBuffer : WINDOW_LENGTH_MAX) - 1;
	
    // Load window into playback buffer
    for (int channel = 0; channel < sampleBuffer.getNumChannels(); channel++)
        buffer->copyFrom(channel, 0, sampleBuffer, channel, samplePanelStartIdx, numSamplesToCopy);

    // Attenuate according to velocity
    currentVelocity = map(float(velocity), 0.0, 127.0, 0.0, 1.0);
    buffer->applyGain(currentVelocity);

    // Set note number
    this->noteNumber = noteNumber;
	
    // Start envelope
    if(adsrMode)
        adsr.noteOn();
}

void Voice::noteOff()
{
    if (adsrMode)
        adsr.noteOff();
}

void Voice::play(AudioBuffer<float>& mainBuffer, int samplePanelStartIdx, int windowLength, bool windowChanged, float pitchWheelValue)
{
    // Always check if adsr is done, if so, reset buffer position, and release note slot
    if (adsrMode && !adsr.isActive()) {
        resetVoice();
        return;
    }

    // Update the playback buffer if the window changed
    if (windowChanged) {
        // Reset playback buffer
        buffer->clear();

    	// Check if window length is out of range
    	const auto numSamplesInBuffer = sampleBuffer.getNumSamples();
	    auto numSamplesToCopy = windowLength;
		if(samplePanelStartIdx + numSamplesToCopy > sampleBuffer.getNumSamples())
	        numSamplesToCopy = sampleBuffer.getNumSamples() - numSamplesToCopy;
    	// If the window is made very large in a small file then use the end of the sample buffer instead
		if(numSamplesToCopy < 0)
			numSamplesToCopy = (WINDOW_LENGTH_MAX > numSamplesInBuffer ? numSamplesInBuffer : WINDOW_LENGTH_MAX) - 1;
    	
        // Load window into playback buffer
        for (int channel = 0; channel < sampleBuffer.getNumChannels(); channel++)
            buffer->copyFrom(channel, 0, sampleBuffer, channel, samplePanelStartIdx, numSamplesToCopy);

    	// Update delay object
        delay.windowChanged();

    	 // Reset buffer position
        bufferPosition = 0;
    }

	// Update delay object
	// This is necessary to do in every audio frame in order to respond to pitch wheel changes
    delay.prepareFineTune(currentFrequency, pitchBendRange, pitchWheelValue);

    // Get number of samples to copy - this corresponds to the block size specified by the plugin host
    auto numSamplesToCopy = mainBuffer.getNumSamples();
    auto isBufferEndReached = false;
    if (bufferPosition + numSamplesToCopy >= buffer->getNumSamples()) {
        // Check whether the end of the capture buffer is reached
        numSamplesToCopy = buffer->getNumSamples() - bufferPosition;
        // Stop playback for next block: In ADSR mode this will set the buffer position to bufferLength / 2 in order to keep the sound
    	// In normal mode this will simply release the current voice
        isBufferEndReached = true;
    }

    // Load windowed data into process buffer
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
    // If mode is ADSR, reset the buffer position to bufferLength / 2 to loop
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
	// Release note slot in global map
    endNoteInMap();
	// Reset buffer position
    bufferPosition = NOT_PLAYING;
	// Clear buffers
    buffer->clear();
    processBuffer->clear();
}

void Voice::endNoteInMap() {
	// Find current voice in map
    for (int& nn : noteNumberForVoice) {
        if (nn == noteNumber) {
        	// Reset to not playing
            nn = NOT_PLAYING;
            return;
        }
    }
	// Reset note number in voice
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

void Voice::setPitchBendRange(int pitchBendRange)
{
    this->pitchBendRange = pitchBendRange;
}

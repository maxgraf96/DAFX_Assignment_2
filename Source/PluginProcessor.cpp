/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

Dafx_assignment_2AudioProcessor::Dafx_assignment_2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Initialise sample panel
    samplePanel.reset(new SamplePanel(windowLength));
    sampleBuffer = samplePanel->getSampleBuffer();

    // Initialise AudioParameters
    windowLengthParam = new AudioParameterFloat(
        "position",
        "Position",
        0.0,
        1.0,
        0.1);
    delayTimeParam = new AudioParameterInt(
        "delayTime",
        "Delay Time",
        0,
        48000 / 50, // 50Hz
        960);
    delayFeedbackParam = new AudioParameterFloat(
        "delayFeedback",
        "Delay Feedback",
        0.0,
        2.0,
        1.0);
    delayWetParam = new AudioParameterFloat(
        "delayWet",
        "Delay Wet",
        0.0,
        1.0,
        1.0);
    
    addParameter(windowLengthParam);
    addParameter(delayTimeParam);
    addParameter(delayFeedbackParam);
    addParameter(delayWetParam);

    // Initialise delay
    delay.reset(new Delay());

    // Initialise midi
    midiStartTime = Time::getMillisecondCounterHiRes() * 0.001;
}

Dafx_assignment_2AudioProcessor::~Dafx_assignment_2AudioProcessor()
{
}

const String Dafx_assignment_2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Dafx_assignment_2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Dafx_assignment_2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Dafx_assignment_2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Dafx_assignment_2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Dafx_assignment_2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Dafx_assignment_2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Dafx_assignment_2AudioProcessor::setCurrentProgram (int index)
{
}

const String Dafx_assignment_2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Dafx_assignment_2AudioProcessor::changeProgramName (int index, const String& newName)
{
}

void Dafx_assignment_2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    samplePanel->setSampleRate(sampleRate);

    // Prepare delay object
    auto processContext = new juce::dsp::ProcessSpec();
    processContext->maximumBlockSize = samplesPerBlock;
    processContext->numChannels = 2;
    processContext->sampleRate = sampleRate;

    // Set n_frames
    // auto blockSRRatio = samplesPerBlock / sampleRate;
    // double captureTimeInSeconds = 0.1;
    // n_frames = int(captureTimeInSeconds / blockSRRatio);

    // Setup capture buffer
    playbackBuffer.reset(new AudioBuffer<float>(2, int(2 * sampleRate)));

    delay->prepare(*processContext);

    // Prepare pitch detection object
    dywapitch_inittracking(&pitchtracker);
}

void Dafx_assignment_2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Dafx_assignment_2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Dafx_assignment_2AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    if (sampleBuffer->getNumChannels() > 0) {
        // Get current window length from parameter
        windowLength = map(*windowLengthParam, 0.0, 1.0, WINDOW_LENGTH_MIN, WINDOW_LENGTH_MAX);
        int totalSampleLength = sampleBuffer->getNumSamples();
        int halfWindow = int(windowLength / 2);
        // Get currently selected position in sample (global, coming from SamplePanel)
        // This is the position the user selected
        int position = round(samplePanel->getSamplePosition() * getSampleRate());
        // Hard limit global position
        if (position + halfWindow >= totalSampleLength) {
            position = totalSampleLength - halfWindow;
        }

        // Calculate window around this position
        samplePanelStartIdx = position - halfWindow;
        samplePanelStartIdx = samplePanelStartIdx > 0 ? samplePanelStartIdx : 0;
        int end = position + halfWindow;
        end = end > totalSampleLength ? totalSampleLength : end;

        // This will hit once a sample was captured and a key is pressed
        if (playFromPlaybackBuffer) {
            buffer.clear();

            // Get number of samples to copy
            auto numSamplesToCopy = numSamples;
            if (playbackBufferSamplePosition + numSamples >= playbackBuffer->getNumSamples()) {
                // If we are at the end of the capture buffer
                numSamplesToCopy = playbackBufferSamplePosition + numSamples - playbackBuffer->getNumSamples();
                // Stop playback for next block
                playFromPlaybackBuffer = false;
            }

            for (int channel = 0; channel < totalNumOutputChannels; channel++) {
                buffer.copyFrom(channel, 0, *playbackBuffer, channel, playbackBufferSamplePosition, numSamplesToCopy);
            }

            // Update sample counter
            playbackBufferSamplePosition += numSamples;
        }
    }

    // Code for "playing" a small windowed part of the loaded signal
    // Through playing and processing the sound it is captured in the captureBuffer object
    // which is subsequently used to determine the pitch and synthesise moooar sound
    //if (sampleBuffer->getNumChannels() > 0 && isPlaying) {
    //    // Get current window length from parameter
    //    windowLength = map(*windowLengthParam, 0.0, 1.0, WINDOW_LENGTH_MIN, WINDOW_LENGTH_MAX);
    //    int totalSampleLength = sampleBuffer->getNumSamples();
    //    int halfWindow = int(windowLength / 2);
    //    // Get currently selected position in sample (global, coming from SamplePanel)
    //    // This is the position the user selected
    //    int position = round(samplePanel->getSamplePosition() * getSampleRate());
    //    // Hard limit global position
    //    if (position + halfWindow >= totalSampleLength) {
    //        position = totalSampleLength - halfWindow;
    //    }
    //    // Calculate window around this position
    //    int samplePanelStartIdx = position - halfWindow;
    //    samplePanelStartIdx = samplePanelStartIdx > 0 ? samplePanelStartIdx : 0;
    //    int end = position + halfWindow;
    //    end = end > totalSampleLength ? totalSampleLength : end;
    //    if (playingPosition >= halfWindow || samplePanelStartIdx + playingPosition >= end) {
    //        playingPosition = 0; 
    //        // Stop playing
    //        isPlaying = false;
    //    }
    //    // Limit numSamples so they can't go out of bounds at the end
    //    if (samplePanelStartIdx + playingPosition + numSamples > totalSampleLength) {
    //        numSamples = samplePanelStartIdx + playingPosition + numSamples - totalSampleLength;
    //    } 
    //    // Check if it's a mono or stereo sample
    //    bool stereo = sampleBuffer->getNumChannels() > 1;
    //    // Copy audio data from sample buffer to plugin buffer
    //    for (int channel = 0; channel < totalNumOutputChannels; channel++) {
    //        buffer.copyFrom(channel, 0, *sampleBuffer, stereo ? channel : 0, samplePanelStartIdx + playingPosition, numSamples);
    //    }
    //    // Update playing position
    //    playingPosition += numSamples;
    //}

    // Feed signal into delay line
    delay->process(buffer);

    // Handle MIDI
    MidiBuffer processedMidi;
    int time;
    MidiMessage m;
    int noteOnVel = 120;

    for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
    {
        if (m.isNoteOn())
        {
            uint8 newVel = (uint8) noteOnVel;
            m = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), newVel);

            playNote(m.getNoteNumber());
        }
        else if (m.isNoteOff())
        {
            // Maybe later
        }

        processedMidi.addEvent(m, time);
    }

    midiMessages.swapWith(processedMidi);
}

void Dafx_assignment_2AudioProcessor::playNote(int noteNumber)
{
    // Reset capture buffer sample position
    playbackBufferSamplePosition = 0;

    // Reset playback buffer
    playbackBuffer->clear();

    // Convert note number to frequency
    auto frequency = MidiMessage::getMidiNoteInHertz(noteNumber);

    // Convert frequency to delay time (seconds)
    double delayTime = 1.0 / frequency;

    // Convert delay time seconds to samples
    int delayTimeSamples = round(delayTime * getSampleRate());

    // Set delay time for both channels
    setDelayTime(delayTimeSamples);

    // Load window into playback buffer
    for (int channel = 0; channel < sampleBuffer->getNumChannels(); channel++)
        playbackBuffer->copyFrom(channel, 0, *sampleBuffer, channel, samplePanelStartIdx, windowLength);

    playFromPlaybackBuffer = true;
}

void Dafx_assignment_2AudioProcessor::setDelayTime(int delayTimeSamples) {
    delay->setDelayTime(0, delayTimeSamples);
    delay->setDelayTime(1, delayTimeSamples);
}

void Dafx_assignment_2AudioProcessor::setDelayFeedback(float delayFeedback) {
    delay->setFeedback(delayFeedback);
}

void Dafx_assignment_2AudioProcessor::setDelayWet(float delayWet) {
    delay->setWetLevel(delayWet);
}

SamplePanel* Dafx_assignment_2AudioProcessor::getSamplePanel()
{
    return samplePanel.get();
}

//==============================================================================
bool Dafx_assignment_2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Dafx_assignment_2AudioProcessor::createEditor()
{
    return new Dafx_assignment_2AudioProcessorEditor (*this);
}

//==============================================================================
void Dafx_assignment_2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Dafx_assignment_2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Dafx_assignment_2AudioProcessor();
}

// Maybe needed later
// Estimate pitch
//currentPitch = dywapitch_computepitch(&pitchtracker, getSampleRate(), captureBufferCopy.get(), 0, numSamples * n_frames);
//// Print average pitch
//if (currentPitch > 0.0) {
//    auto note = int(log(currentPitch / 440.0) / log(2) * 12 + 69);
//    auto octave = note / 12 - 1;
//    juce::String notes = "C C#D D#E F F#G G#A A#B ";
//    auto note_str = notes.substring((note % 12) * 2, (note % 12) * 2 + 2) + juce::String(octave);
//    DBG(note_str);
//}
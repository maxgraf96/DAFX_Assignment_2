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
    , parameters(*this,
        nullptr, // No undo manager
        Identifier("DAFX2"),
        {
            std::make_unique<AudioParameterFloat>(
                "position",
                "Sample Position",
                0.0,
                1.0,
                0.0
                ),
            std::make_unique<AudioParameterFloat>(
                "windowLength",
                "Window Length",
                0.0,
                1.0,
                0.1),
            std::make_unique<AudioParameterFloat>(
                "delayFeedback",
                "Delay Feedback",
                0.0,
                2.0,
                1.0),
            std::make_unique<AudioParameterFloat>(
                "delayWet",
                "Delay Wet",
                0.0,
                1.0,
                1.0)
        })
{
    // Initialise sample panel
    samplePanel.reset(new SamplePanel(windowLength, parameters));
    sampleBuffer = samplePanel->getSampleBuffer();

    // Initialise listeners for parameters
    parameters.addParameterListener("position", this);
    parameters.addParameterListener("windowLength", this);
    parameters.addParameterListener("delayFeedback", this);
    parameters.addParameterListener("delayWet", this);

    positionParam = parameters.getRawParameterValue("position");
    windowLengthParam = parameters.getRawParameterValue("windowLength");
    delayFeedbackParam = parameters.getRawParameterValue("delayFeedback");
    delayWetParam = parameters.getRawParameterValue("delayWet");
}

Dafx_assignment_2AudioProcessor::~Dafx_assignment_2AudioProcessor()
{
    voices.clear();
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
    auto delayProcessContext = new juce::dsp::ProcessSpec();
    delayProcessContext->maximumBlockSize = samplesPerBlock;
    delayProcessContext->numChannels = 2;
    delayProcessContext->sampleRate = sampleRate;

    // Prepare pitch detection object
    // dywapitch_inittracking(&pitchtracker);

    // Initialise 16 voices
    for (int voice = 0; voice < NUM_VOICES; voice++) {
        voices.push_back(std::unique_ptr<Voice>(new Voice(sampleBuffer, *delayProcessContext, int(2 * sampleRate))));
        voices[voice]->setDelayFeedback(*delayFeedbackParam);
        voices[voice]->setDelayWet(*delayWetParam);
    }
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
    
    // ------------------------ MIDI ------------------------ 
    MidiBuffer processedMidi;
    int time;
    MidiMessage m;
    int noteOnVel = 120;

    for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
    {
        if (m.isNoteOn())
        {
            uint8 newVel = (uint8)noteOnVel;
            m = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), newVel);

            auto noteNumber = m.getNoteNumber();
            // TODO: Find next free voice and trigger note
            for (auto&& voice : voices) {
                if (!voice->isPlaying()) {
                    voice->noteOn(noteNumber, samplePanelStartIdx, windowLength);
                    goto rest;
                }
            }
        }
        else if (m.isNoteOff())
        {
            // Maybe later
        }
    rest:
        processedMidi.addEvent(m, time);
    }
    midiMessages.swapWith(processedMidi);

    // ------------------------ SOUND ------------------------ 
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
        buffer.clear();

        for (auto&& voice : voices) {
            if (voice->isPlaying()) {
                voice->play(buffer);
            }
        }
    }
}

void Dafx_assignment_2AudioProcessor::setDelayFeedback(float delayFeedback) {
    for (auto&& voice : voices)
        voice->setDelayFeedback(delayFeedback);
}

void Dafx_assignment_2AudioProcessor::setDelayWet(float delayWet) {
    for (auto&& voice : voices)
        voice->setDelayWet(delayWet);
}

void Dafx_assignment_2AudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == "position") {
        samplePanel->setSamplePosition(newValue);
    }
    if (parameterID == "windowLength") {
        samplePanel->setWindowLength(newValue);
    }
    if (parameterID == "delayFeedback") {
        setDelayFeedback(newValue);
    }
    if (parameterID == "delayWet") {
        setDelayWet(newValue);
    }
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
    return new Dafx_assignment_2AudioProcessorEditor (*this, parameters);
}

//==============================================================================
void Dafx_assignment_2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // Store state when closing plugin
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Dafx_assignment_2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore saved state
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));

    // Have to manually set here because state loading happens after all components are constructed
    Value path = parameters.state.getPropertyAsValue("currentFilePath", nullptr);
    auto path_str = path.getValue().toString();
    if (path_str != "") {
        samplePanel->setCurrentFilePath(path_str);

        // Update position as well
        samplePanel->setSamplePositionAbsolute(*positionParam);
    } 
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Dafx_assignment_2AudioProcessor();
}
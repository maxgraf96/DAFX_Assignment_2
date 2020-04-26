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
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    )
#endif
    ,parameters(*this,
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
            0.9,
            1.0,
            0.98),
        std::make_unique<AudioParameterBool>(
            "mode",
            "Mode",
            false),
        std::make_unique<AudioParameterInt>(
            "attack",
            "Attack",
            0,
            2000,
            0
        ),
        std::make_unique<AudioParameterInt>(
            "decay",
            "Decay",
            0,
            1000,
            0
        ),
        std::make_unique<AudioParameterFloat>(
            "sustain",
            "Sustain",
            0.0,
            1.0,
            1.0
        ),
        std::make_unique<AudioParameterInt>(
            "release",
            "Release",
            0,
            3000,
            0
        ),
        std::make_unique<AudioParameterBool>(
            "dynamicVelocity",
            "Dynamic velocity",
            false),
        std::make_unique<AudioParameterBool>(
            "adaptiveDecay",
            "Adaptive decay",
            false)
        })
{
    // Initialise sample panel
    samplePanel.reset(new SamplePanel(windowLength, parameters));
    sampleBuffer = samplePanel->getSampleBuffer();

    // Initialise listeners for parameters
    parameters.addParameterListener("position", this);
    parameters.addParameterListener("windowLength", this);
    parameters.addParameterListener("delayFeedback", this);
    parameters.addParameterListener("mode", this);
    parameters.addParameterListener("attack", this);
    parameters.addParameterListener("decay", this);
    parameters.addParameterListener("sustain", this);
    parameters.addParameterListener("release", this);
    parameters.addParameterListener("dynamicVelocity", this);
    parameters.addParameterListener("adaptiveDecay", this);

    positionParam = parameters.getRawParameterValue("position");
    windowLengthParam = parameters.getRawParameterValue("windowLength");
    delayFeedbackParam = parameters.getRawParameterValue("delayFeedback");
    modeParam = parameters.getRawParameterValue("mode");
    attackParam = parameters.getRawParameterValue("attack");
    decayParam = parameters.getRawParameterValue("decay");
    sustainParam = parameters.getRawParameterValue("sustain");
    releaseParam = parameters.getRawParameterValue("release");
    dynamicVelocityParam = parameters.getRawParameterValue("dynamicVelocity");
    adaptiveDecayParam = parameters.getRawParameterValue("adaptiveDecay");
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

    // Clear voices (for some reason prepareToPlay() can be called twice per startup)
    voices.clear();

    // Initialise NUM_VOICES (currently 16) voices
    bool isADSRMode = *modeParam > 0.0 ? true : false;
    bool isAdaptiveDecay = !isADSRMode && *adaptiveDecayParam > 0.0f;
    for (int i = 0; i < NUM_VOICES; i++) {
        voices.push_back(std::unique_ptr<Voice>(new Voice(sampleBuffer, *delayProcessContext, int(2 * sampleRate), noteNumberForVoice)));
        voices[i]->setDelayFeedback(*delayFeedbackParam);
        voices[i]->setDelayWet(1.0);
        voices[i]->setADSRParams(adsrParams);
        voices[i]->setADSRMode(isADSRMode);
        voices[i]->setAdaptiveDecay(isAdaptiveDecay);

        // Initialise note number to voice map
        // -1 means "not playing", if a voice is playing this array will contain the MIDI
        // note number the voice is currently playing
        noteNumberForVoice[i] = -1;
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
    // Some voice parameters (e.g. adsr mode) can't be changed during playback at the moment
    // This flag should be set if the voices need to be changed
    // Currently these changes include adsr mode switching
    if (shouldVoicesChange) {
        changeVoices();
        return;
    }
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    
    // ------------------------ MIDI ------------------------ 
    MidiBuffer processedMidi;
    int time;
    MidiMessage m;

    for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
    {
        auto noteNumber = m.getNoteNumber();
        if (m.isNoteOn())
        {
            m = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), m.getVelocity());
            uint8 velocity = m.getVelocity();
            if (*dynamicVelocityParam == 0.0f) {
                velocity = 127;
            }

            // Check if note already exists. If so, retrigger
            bool isRetrigger = false;
            int selectedVoiceIdx = -1;
            for (int i = 0; i < NUM_VOICES; i++) {
                if (noteNumberForVoice[i] == noteNumber) {
                    // Current incoming note is alreay playing => retrigger
                    isRetrigger = true;
                    selectedVoiceIdx = i;
                    break;
                }
            }

            if (isRetrigger) {
                voices[selectedVoiceIdx]->noteOn(noteNumber, velocity, samplePanelStartIdx, windowLength);
            }
            else {
                // If it's a completely new note find the next free voice and trigger
                for (int i = 0; i < NUM_VOICES; i++) {
                    if (noteNumberForVoice[i] == Voice::NOT_PLAYING) {
                        voices[i]->noteOn(noteNumber, velocity, samplePanelStartIdx, windowLength);
                        selectedVoiceIdx = i;
                        break;
                    }
                }
            }

            // Set in map
            if (selectedVoiceIdx > -1) {
                noteNumberForVoice[selectedVoiceIdx] = noteNumber;
            }
            else {
                // More incoming notes than available slot in voices array
                // Either don't play or replace longest playing note
            }
        }
        if (m.isNoteOff())
        {
            // Get voice for note number
            for (int i = 0; i < NUM_VOICES; i++) {
                if (noteNumberForVoice[i] == noteNumber) {
                    voices[i]->noteOff();
                    break;
                }
            }
        }
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

        for (int i = 0; i < NUM_VOICES; i++) {
            if (noteNumberForVoice[i] > Voice::NOT_PLAYING)
                voices[i]->play(buffer);
        }
    }
}

void Dafx_assignment_2AudioProcessor::setDelayFeedback(float delayFeedback) {
    for (auto&& voice : voices)
        voice->setDelayFeedback(delayFeedback);
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
    if (parameterID == "mode") {
        // This check is necessary to handle mode change via automation
        // Otherwise "non-normalisable" values are set (as it's a bool param) and JUCE complains
        if (newValue > 0.0f)
            *modeParam = 1.0f;
        else
            *modeParam = 0.0f;
        // Stop playback
        shouldVoicesChange = true;
    }
    if (parameterID == "attack") {
        adsrParams.attack = newValue * 0.001;
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if (parameterID == "decay") {
        adsrParams.decay = newValue * 0.001;
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if (parameterID == "sustain") {
        adsrParams.sustain = newValue;
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if (parameterID == "release") {
        adsrParams.release = newValue * 0.001;
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if(parameterID == "adaptiveDecay") {
        if (newValue > 0.0f)
            *adaptiveDecayParam = 1.0f;
        else
            *adaptiveDecayParam = 0.0f;
        shouldVoicesChange = true;
    }
}

void Dafx_assignment_2AudioProcessor::changeVoices()
{
    bool isADSRMode = *modeParam > 0.0;
    bool isAdaptiveDecay = !isADSRMode && *adaptiveDecayParam > 0.0f;
    // Set voices accordingly
    for (auto&& voice : voices) {
        voice->setADSRMode(isADSRMode);
        voice->setAdaptiveDecay(isAdaptiveDecay);
    }
    if (isADSRMode) {
        // Pad mode
        // Set delay feedback to 1.0 as sound over time will be controlled by ADSR envelope
        parameters.getParameterAsValue("delayFeedback").setValue(1.0);
    }
    else {
        // String mode
        parameters.getParameterAsValue("delayFeedback").setValue(0.99);
    }

    // Re-enable sound
    shouldVoicesChange = false;
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

    // Convert from ms to seconds
    adsrParams.attack = *attackParam * 0.001;
    adsrParams.decay = *decayParam * 0.001;
    adsrParams.sustain = *sustainParam;
    adsrParams.release = *releaseParam * 0.001;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Dafx_assignment_2AudioProcessor();
}
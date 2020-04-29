/*
  ==============================================================================

    Plugin processor for DAFX_Assignment_2.

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
	// Setup all audio parameters
    , parameters(*this,
        nullptr, // No undo manager
        Identifier("DAFX2"),
        {
        std::make_unique<AudioParameterFloat>(
            "position",
            "Sample Position",
            0.0f,
            1.0f,
            0.0f
            ),
        std::make_unique<AudioParameterInt>(
            "windowLength",
            "Window Length",
            WINDOW_LENGTH_MIN,
            WINDOW_LENGTH_MAX,
            WINDOW_LENGTH_MIN),
        std::make_unique<AudioParameterFloat>(
            "delayFeedback",
            "Delay Feedback",
            delayFeedbackRange,
            0.98f),
        std::make_unique<AudioParameterBool>(
            "mode",
            "Mode",
            false),
        std::make_unique<AudioParameterFloat>(
            "attack",
            "Attack",
            adsrRange,
            0.0f
        ),
        std::make_unique<AudioParameterFloat>(
            "decay",
            "Decay",
            adsrRange,
            0.0f
        ),
        std::make_unique<AudioParameterFloat>(
            "sustain",
            "Sustain",
            0.0f,
            1.0f,
            1.0f
        ),
        std::make_unique<AudioParameterFloat>(
            "release",
            "Release",
            adsrRange,
            0.0f
        ),
        std::make_unique<AudioParameterBool>(
            "dynamicVelocity",
            "Dynamic velocity",
            false),
        std::make_unique<AudioParameterBool>(
            "adaptiveDecay",
            "Adaptive decay",
            false),
        std::make_unique<AudioParameterInt>(
            "pitchBendRange",
            "Pitch Bend Range",
            0,
            24,
            7
        ),
        std::make_unique<AudioParameterFloat>(
            "mainFilterCutoff",
            "Main Filter Cutoff Frequency",
            mainFilterCutoffRange,
            18000.0f
            ),
        std::make_unique<AudioParameterFloat>(
            "mainFilterQ",
            "Main Filter Q",
            0.001f,
            3.0f,
            0.707106781f // sqrt(2) / 2
            ),
        std::make_unique<AudioParameterFloat>(
            "mainOutputGain",
            "Main output gain",
            -30.0f,
            0.0f,
            0.0f
            )
        })
{
    // Initialise sample panel
    samplePanel.reset(new SamplePanel(parameters));
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
    parameters.addParameterListener("pitchBendRange", this);
    parameters.addParameterListener("mainFilterCutoff", this);
    parameters.addParameterListener("mainFilterQ", this);

	// Create pointers to parameter values
    positionParam = parameters.getRawParameterValue("position");
    windowLengthParam = parameters.getRawParameterValue("windowLength");
    delayFeedbackParam = parameters.getRawParameterValue("delayFeedback");
    adsrModeParam = parameters.getRawParameterValue("mode");
    attackParam = parameters.getRawParameterValue("attack");
    decayParam = parameters.getRawParameterValue("decay");
    sustainParam = parameters.getRawParameterValue("sustain");
    releaseParam = parameters.getRawParameterValue("release");
    dynamicVelocityParam = parameters.getRawParameterValue("dynamicVelocity");
    adaptiveDecayParam = parameters.getRawParameterValue("adaptiveDecay");
    pitchBendRangeParam = parameters.getRawParameterValue("pitchBendRange");
    mainFilterCutoffParam = parameters.getRawParameterValue("mainFilterCutoff");
    mainFilterQParam = parameters.getRawParameterValue("mainFilterQ");
    mainOutputGainParam = parameters.getRawParameterValue("mainOutputGain");

    // Set Open Sans as default font
    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Open Sans");
}

Dafx_assignment_2AudioProcessor::~Dafx_assignment_2AudioProcessor()
{
	// Clear voices vector
    voices.clear();
}

void Dafx_assignment_2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Set sample rate for sample panel
    samplePanel->setSampleRate(sampleRate);

    // Prepare process context object used to initialise voices and delay objects
    const auto processContext = new dsp::ProcessSpec();
    processContext->maximumBlockSize = samplesPerBlock;
    processContext->numChannels = 2; // Currently limited to stereo
    processContext->sampleRate = sampleRate;

    // Clear voices (for some reason prepareToPlay() can be called twice per startup)
    voices.clear();

    // Initialise NUM_VOICES (currently 16) voices
	// Check if ADSR mode is enabled
    const bool isADSRMode = *adsrModeParam > 0.0 ? true : false;
	// Check if adaptive decay is enabled
    const bool isAdaptiveDecay = !isADSRMode && *adaptiveDecayParam > 0.0f;
	// Get ADSR params from state
	adsrParams.attack = *attackParam * 0.001;
	adsrParams.decay = *decayParam * 0.001;
	adsrParams.sustain = *sustainParam;
	adsrParams.release = *releaseParam * 0.001;
	
    for (int i = 0; i < NUM_VOICES; i++) {
    	// Create new voice
        voices.push_back(std::make_unique<Voice>(*sampleBuffer, *processContext, int(2 * sampleRate),
                                                 noteNumberForVoice));
        // Configure voice
    	voices[i]->setDelayFeedback(*delayFeedbackParam);
        voices[i]->setDelayWet(1.0);
        voices[i]->setADSRParams(adsrParams);
        voices[i]->setADSRMode(isADSRMode);
        voices[i]->setAdaptiveDecay(isAdaptiveDecay);
        voices[i]->setPitchBendRange(static_cast<int>(*pitchBendRangeParam));

        // Initialise voice in note-number-to-voice map
        // -1 means "not playing", if a voice is playing this array will contain the MIDI
        // note number the voice is currently playing
        noteNumberForVoice[i] = NOT_PLAYING;
    }

    // Setup main lowpass filters
    for (auto& f : mainLowpassFilters)
    {
        f.prepare(*processContext);
        f.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, *mainFilterCutoffParam, *mainFilterQParam);
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
    // These changes include adsr mode switching and adaptive decay toggling
    if (shouldVoicesChange) {
        changeVoices();
        return;
    }
	
    ScopedNoDenormals noDenormals;

	// Get number of samples in buffer
    const auto numSamples = buffer.getNumSamples();
    
    // ------------------------ MIDI ------------------------ 
    // Swap buffer for MIDI messages
    MidiBuffer processedMidi;
    // Placeholder for MIDI times
    int time;
    // Placeholder for the current MIDI message
    MidiMessage m;

    // Iterate over all incoming MIDI messages
    for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
    {
        // Get the MIDI note number
        const auto noteNumber = m.getNoteNumber();
        // Note on handling
        if (m.isNoteOn())
        {
            m = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), m.getVelocity());
            uint8 velocity = m.getVelocity();
            // If dynamic velocity handling is enabled the incoming MIDI velocity is used to attenuate the buffer
            // If it is disabled the maximum MIDI velocity of 127 is used
            if (*dynamicVelocityParam == 0.0f) {
                velocity = 127;
            }

            // Check if note already exists. If so, retrigger
            bool isRetrigger = false;
            // Selected voice index will contain the index of the voice selected for trigger/retriggering
            int selectedVoiceIdx = NOT_PLAYING;
            // Iterate over array of voices and check if the incoming MIDI note is already playing
            for (int i = 0; i < NUM_VOICES; i++) {
                if (noteNumberForVoice[i] == noteNumber) {
                    // Current incoming note is alreay playing => retrigger
                    isRetrigger = true;
                    selectedVoiceIdx = i;
                    break;
                }
            }

            // If the incoming note is already playing simply retrigger it
            if (isRetrigger) {
                voices[selectedVoiceIdx]->noteOn(noteNumber, velocity, samplePanelStartIdx, windowLength);
            }
            // If it's a completely new note find the next free voice and trigger
            else {
                for (int i = 0; i < NUM_VOICES; i++) {
                    if (noteNumberForVoice[i] == NOT_PLAYING) {
                        voices[i]->noteOn(noteNumber, velocity, samplePanelStartIdx, windowLength);
                        selectedVoiceIdx = i;
                        break;
                    }
                }
            }

            // Set in map
            if (selectedVoiceIdx > NOT_PLAYING) {
                noteNumberForVoice[selectedVoiceIdx] = noteNumber;
            }
            else {
                // More incoming notes than available slot in voices array
                // Either don't play or replace longest playing note
                // Current behaviour is to ignore new incoming notes if all slots (16) are taken
            }
        }
        // Note off handling
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

        // Glissandi/pitch-bend controlled by MIDI pitch wheel
        if (m.isPitchWheel()) {
            // Map to [-1.0, 1.0] for tuning in voices
            mappedPitchWheelValue = map(static_cast<float>(m.getPitchWheelValue()), 0.0f, MIDI_PITCH_WHEEL_MAX_VAL, -1.0f, 1.0f);
        }

        processedMidi.addEvent(m, time);
    }
	// Swap MIDI buffers
    midiMessages.swapWith(processedMidi);
    // ------------------------ MIDI end ------------------------ 

    // ------------------------ SOUND ---------------------------
	// Only process if a sample is loaded
    if (sampleBuffer->getNumChannels() > 0) {
        // Get current window length from parameter
        windowLength = *windowLengthParam;
        const int totalSampleLength = sampleBuffer->getNumSamples();
        const int halfWindow = int(windowLength / 2);
        // Get currently selected position in sample (global, coming from SamplePanel)
        // This is the position the user selected
        int position = roundToInt(samplePanel->getSamplePosition() * getSampleRate());
        // Hard limit global position
        // (disallow user from choosing positions that would lead to part of the window being out of the sample range)
        if (position + halfWindow >= totalSampleLength) {
            position = totalSampleLength - halfWindow;
        }
        if (position - halfWindow <= 0) {
            position = 0 + halfWindow;
        }

        // Calculate window around this position
        samplePanelStartIdx = position - halfWindow;
        samplePanelStartIdx = samplePanelStartIdx > 0 ? samplePanelStartIdx : 0;

        // Check if playback window has changed (either by being moved or changed in size)
        // If so, update the voices accordingly
        const bool windowChanged = samplePanelStartIdx != prevSamplePanelStartIdx || windowLength != prevWindowLength;

        // Clear the main output buffer
        buffer.clear();

    	// For each voice currently playing load sound into the main buffer
        for (int i = 0; i < NUM_VOICES; i++) {
            if (noteNumberForVoice[i] > NOT_PLAYING)
                voices[i]->play(buffer, samplePanelStartIdx, windowLength, windowChanged, mappedPitchWheelValue);
        }

        // Update previous sample panel start index and window length
        prevSamplePanelStartIdx = samplePanelStartIdx;
        prevWindowLength = windowLength;

        // Feed through main lowpass filter
        for (auto channel = 0; channel < buffer.getNumChannels(); channel++) {
            const auto writer = buffer.getWritePointer(channel);
            for (int i = 0; i < numSamples; i++) {
                writer[i] = mainLowpassFilters[channel].processSample(writer[i]);
            }
        }

    	// Convert dB to linear and apply to output
        buffer.applyGain(pow(10.0f, *mainOutputGainParam / 20.0f));
    }
    // ------------------------ SOUND end ---------------------------
}

void Dafx_assignment_2AudioProcessor::setDelayFeedback(float delayFeedback) {
    for (auto&& voice : voices)
        voice->setDelayFeedback(delayFeedback);
}

std::array<dsp::IIR::Filter<float>, 2>& Dafx_assignment_2AudioProcessor::getMainLowpassFilters()
{
    return mainLowpassFilters;
}

void Dafx_assignment_2AudioProcessor::updateMainLowpassFilters(float cutoff, float q)
{
	// Apply cutoff and q to both filters (for left and right channel)
    mainLowpassFilters[0].coefficients = mainLowpassFilters[0].coefficients->makeLowPass(getSampleRate(), cutoff, q);
    mainLowpassFilters[1].coefficients = mainLowpassFilters[1].coefficients->makeLowPass(getSampleRate(), cutoff, q);
}

AudioProcessorValueTreeState& Dafx_assignment_2AudioProcessor::getVTS()
{
    return parameters;
}

void Dafx_assignment_2AudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == "position") {
    	// Update position in sample panel
        samplePanel->setSamplePosition(newValue);
    }
    if (parameterID == "windowLength") {
    	// Update window length in sample panel
        samplePanel->setWindowLength(newValue);
    }
    if (parameterID == "delayFeedback") {
    	// Update delay feedback value on all voices
        setDelayFeedback(newValue);
    }
    if (parameterID == "mode") {
        // This check is necessary to handle mode change via automation
        // Otherwise "non-normalisable" values are set (as it's a bool param) and JUCE complains
        if (newValue > 0.0f)
            *adsrModeParam = 1.0f;
        else
            *adsrModeParam = 0.0f;
        // Stop playback
        shouldVoicesChange = true;
    }
    if (parameterID == "attack") {
    	// Convert ms to seconds
        adsrParams.attack = newValue * 0.001;
    	// Apply to all voices
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if (parameterID == "decay") {
        // Convert ms to seconds
        adsrParams.decay = newValue * 0.001;
    	// Apply to all voices
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if (parameterID == "sustain") {
        adsrParams.sustain = newValue;
    	// Apply to all voices
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if (parameterID == "release") {
        // Convert ms to seconds
        adsrParams.release = newValue * 0.001;
    	// Apply to all voices
        for (auto&& voice : voices) {
            voice->setADSRParams(adsrParams);
        }
    }
    if(parameterID == "adaptiveDecay") {
        if (newValue > 0.0f)
            *adaptiveDecayParam = 1.0f;
        else
            *adaptiveDecayParam = 0.0f;
    	// Set breaking voice change flag (stops sound and changes configuration, then re-enables sound)
        shouldVoicesChange = true;
    }
    if (parameterID == "pitchBendRange") {
    	// Apply pitch-bend to all voices
        for (auto&& voice : voices) {
            voice->setPitchBendRange(static_cast<int>(newValue));
        }
    }
    if (parameterID == "mainFilterCutoff") {
    	// Apply to filters
        updateMainLowpassFilters(newValue, *mainFilterQParam);
    }
    if (parameterID == "mainFilterQ") {
    	// Apply to filters
        updateMainLowpassFilters(*mainFilterCutoffParam, newValue);
    }
}

void Dafx_assignment_2AudioProcessor::changeVoices()
{
	// Check if ADSR mode is enabled
    const bool isADSRMode = *adsrModeParam > 0.0;
	// Check if adaptive decay is enabled
    const bool isAdaptiveDecay = !isADSRMode && *adaptiveDecayParam > 0.0f;
    // Set voices accordingly
    for (auto&& voice : voices) {
        voice->setADSRMode(isADSRMode);
        voice->setAdaptiveDecay(isAdaptiveDecay);
    }
    if (isADSRMode) {
        // ADSR mode
        // Set delay feedback to 1.0 as sound over time will be controlled by ADSR envelope
        parameters.getParameterAsValue("delayFeedback").setValue(1.0f);
    }
    else {
        // If disabling ADSR mode, the feedback is currently automatically reset to 0.99
        parameters.getParameterAsValue("delayFeedback").setValue(0.99f);
    }

    // Re-enable sound
    shouldVoicesChange = false;
}

//==============================================================================
bool Dafx_assignment_2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Dafx_assignment_2AudioProcessor::createEditor()
{
    return new Dafx_assignment_2AudioProcessorEditor (*this, parameters, *samplePanel);
}

//==============================================================================
void Dafx_assignment_2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // Store state when closing plugin
    const auto state = parameters.copyState();
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
	// This makes sure that the right file path is injected into the sample panel
	// if the plugin is loaded from a saved state.
    const Value path = parameters.state.getPropertyAsValue("currentFilePath", nullptr);
    auto path_str = path.getValue().toString();
    if (path_str != "") {
        samplePanel->setCurrentFilePath(path_str);

        // Update position as well
        samplePanel->setSamplePositionFromState(*positionParam);
    } 

    // Convert from ms to seconds
    adsrParams.attack = *attackParam * 0.001;
    adsrParams.decay = *decayParam * 0.001;
    adsrParams.sustain = *sustainParam;
    adsrParams.release = *releaseParam * 0.001;

	// Apply ADSR params to all voices
	for(auto&& voice : voices)
	{
		voice->setADSRParams(adsrParams);
	}
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

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Dafx_assignment_2AudioProcessor();
}
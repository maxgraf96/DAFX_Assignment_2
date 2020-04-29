/*
  ==============================================================================

    Delay.h
    Created: 30 Mar 2020 2:28:05pm
    Author:  Music

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayLine.h"
#include "Constants.h"

class Delay
{
public:
	// Default constructor
    Delay();

	// Prepare using the current host configuration
    void prepare(const dsp::ProcessSpec& spec);
	// Process an incoming audio buffer
    void process(AudioBuffer<float>& buffer) noexcept;
	// Reset to default values
    void reset() noexcept;
	// Triggered if the main sample window was changed
	void windowChanged();
	
    size_t getNumChannels() const noexcept;
    void setMaxDelayTime(float newValue);
    void setFeedback(float newValue) noexcept;
    void setWetLevel(float newValue) noexcept;
    void setDelayTime(size_t channel, int newValue);

	// Prepare the delay object for the incoming note
	// This method implements the fine-tune mechanism described in the Karplus-Strong extension paper
    void prepareFineTune(double fundamentalFrequency, int pitchBendRange, float pitchWheelValue);
	// Set the sample rate
    void setSampleRate(double sr);
	// (De-)activate adaptive decay time
    void setAdaptiveDecay(bool isAdaptiveDecay);

private:
	// Currently limited to stereo
    const int maxNumChannels = 2;
	// Delay lines for each channel
    std::array<DelayLine<float>, 2> delayLines;
	// Delay time in samples
    std::array<size_t, 2> delayTimesSample;
	// Delay time in seconds (currently not used)
    std::array<int, 2> delayTimes;
    float feedback = 0.0f;
    float wetLevel = 0.0f;

	// Lowpass filters (one per channel) used to attenuate the signal
	// passing through the delay line
    std::array<dsp::IIR::Filter<float>, 2> filters;
    dsp::IIR::Coefficients<float>::Ptr filterCoefs;

    // Allpass filters for fine-tuning
	// This is based on the technique described in the Karplus-Strong extension paper
    std::array<dsp::IIR::Filter<float>, 2> tuningFilters;
    dsp::IIR::Coefficients<float>::Ptr tuningFilterCoefs;

	// These will be overridden
    double sampleRate = 44.1e3;
    float maxDelayTime = 2.0f;

    void updateDelayLineSize();
    void updateDelayTime() noexcept;
    
    // The tuning coefficient
    double C = -1.0f;
    // The offset used by the fine-tuning allpass filter
	const float epsilon = 0.01;

	// Whether adaptive decay time is enabled
    bool isAdaptiveDecay = false;
    // The decay stretch factor
    float stretchFactor = 1.0f;
};
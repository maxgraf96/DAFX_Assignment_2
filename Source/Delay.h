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
    Delay();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(AudioBuffer<float>& buffer) noexcept;
    void reset() noexcept;
	void windowChanged();
    size_t getNumChannels() const noexcept;
    void setMaxDelayTime(float newValue);
    void setFeedback(float newValue) noexcept;
    void setWetLevel(float newValue) noexcept;
    void setDelayTime(size_t channel, int newValue);

    void prepareFineTune(double fundamentalFrequency, int pitchBendRange, float pitchWheelValue);
    void setSampleRate(double sr);
    void setAdaptiveDecay(bool isAdaptiveDecay);

private:
    const int maxNumChannels = 2;
    std::array<DelayLine<float>, 2> delayLines;
    std::array<size_t, 2> delayTimesSample;
    std::array<int, 2> delayTimes;
    float feedback{ float(0) };
    float wetLevel{ float(0) };

    std::array<juce::dsp::IIR::Filter<float>, 2> filters;
    juce::dsp::IIR::Coefficients<float>::Ptr filterCoefs;

    // Allpass filters for tuning
    std::array<juce::dsp::IIR::Filter<float>, 2> tuningFilters;
    juce::dsp::IIR::Coefficients<float>::Ptr tuningFilterCoefs;

    double sampleRate{ 44.1e3 };
    float maxDelayTime{ float(2) };

    void updateDelayLineSize();

    void updateDelayTime() noexcept;
    
    // Fine-tune mechanism described in the KS extension paper
    float fineTune(float input, float prevInput, float prevOutput);
    // The tuning coefficient
    double C = -1.0f;
    float prevIn = 0.0f;
    float prevOut = 0.0f;

    // The decay stretch factor
    bool isAdaptiveDecay = false;
    float stretchFactor = 1.0f;

};
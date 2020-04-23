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

class Delay
{
public:
    Delay();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(AudioBuffer<float>& buffer) noexcept;
    void reset() noexcept;
    size_t getNumChannels() const noexcept;
    void setMaxDelayTime(float newValue);
    void setFeedback(float newValue) noexcept;
    void setWetLevel(float newValue) noexcept;
    void setDelayTime(size_t channel, int newValue);

    void setFundamentalFrequency(double f0);
    void setSampleRate(double sr);

private:
    const int maxNumChannels = 2;
    std::array<DelayLine<float>, 2> delayLines;
    std::array<size_t, 2> delayTimesSample;
    std::array<int, 2> delayTimes;
    float feedback{ float(0) };
    float wetLevel{ float(0) };

    std::array<juce::dsp::IIR::Filter<float>, 2> filters;
    juce::dsp::IIR::Coefficients<float>::Ptr filterCoefs;

    double sampleRate{ 44.1e3 };
    float maxDelayTime{ float(2) };
    double fundamentalFrequency = -1.0;

    void updateDelayLineSize();

    void updateDelayTime() noexcept;
    
    // Fine-tune mechanism described in the KS extension paper
    float fineTune(float input, float prevInput, float prevOutput);
};
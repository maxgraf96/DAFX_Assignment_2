/*
  ==============================================================================

    Delay.cpp
    Created: 30 Mar 2020 3:09:37pm
    Author:  Music

  ==============================================================================
*/

#include "Delay.h"

Delay::Delay()
{
    setMaxDelayTime(2.0f);
    setDelayTime(0, 0.7f);
    setDelayTime(1, 0.5f);
    setWetLevel(0.8f);
    setFeedback(0.5f);
}

void Delay::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels <= maxNumChannels);
    sampleRate = (float)spec.sampleRate;
    updateDelayLineSize();
    updateDelayTime();

    // Use first order LP for now
    filterCoefs = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, float(1e3));

    for (auto& f : filters)
    {
        f.prepare(spec);
        f.coefficients = filterCoefs;
    }
}

void Delay::process(AudioBuffer<float>& buffer) noexcept
{
    auto inputBuffer = buffer.getArrayOfWritePointers();
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto input = inputBuffer[ch];
        auto output = inputBuffer[ch];
        auto& dline = delayLines[ch];
        auto delayTime = delayTimesSample[ch];
        auto& filter = filters[ch];

        for (size_t i = 0; i < numSamples; ++i)
        {
            auto delayedSample = filter.processSample(dline.get(delayTime));
            auto inputSample = input[i];
            auto dlineInputSample = std::tanh(inputSample + feedback * delayedSample);
            dline.push(dlineInputSample);
            auto outputSample = inputSample + wetLevel * delayedSample;
            output[i] = outputSample;
        }
    }
}

void Delay::reset() noexcept
{
    for (auto& f : filters)
        f.reset();

    for (auto& dline : delayLines)
        dline.clear();
}

size_t Delay::getNumChannels() const noexcept
{
    return delayLines.size();
}

void Delay::setMaxDelayTime(float newValue)
{
    jassert(newValue > float(0));
    maxDelayTime = newValue;
    updateDelayLineSize();
}

void Delay::setFeedback(float newValue) noexcept
{
    jassert(newValue >= float(0) && newValue <= float(1));
    feedback = newValue;
}

void Delay::setWetLevel(float newValue) noexcept
{
    jassert(newValue >= float(0) && newValue <= float(1));
    wetLevel = newValue;
}

void Delay::setDelayTime(size_t channel, float newValue)
{
    if (channel >= getNumChannels())
    {
        jassertfalse;
        return;
    }

    jassert(newValue >= float(0));
    delayTimes[channel] = newValue;

    updateDelayTime();
}

void Delay::updateDelayLineSize() {
    auto delayLineSizeSamples = (size_t)std::ceil(maxDelayTime * sampleRate);

    for (auto& dline : delayLines)
        dline.resize(delayLineSizeSamples);
}

void Delay::updateDelayTime() noexcept {
    for (size_t ch = 0; ch < maxNumChannels; ++ch)
        delayTimesSample[ch] = (size_t)juce::roundToInt(delayTimes[ch] * sampleRate); // [4]
}
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
    setWetLevel(1.0f);
    setFeedback(1.0f);
}

void Delay::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels <= maxNumChannels);
    sampleRate = (float)spec.sampleRate;
    updateDelayLineSize();
    updateDelayTime();

    // Use first order LP for now
    filterCoefs = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, float(18000));

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
        auto delayTime = delayTimes[ch];
        auto& filter = filters[ch];

        auto prevC = 0.0;

        for (size_t i = 0; i < numSamples; ++i)
        {
            auto delayedSample = filter.processSample(dline.get(delayTime));
            auto inputSample = input[i];
            auto dlineInputSample = std::tanh(inputSample + feedback * delayedSample);
            dline.push(dlineInputSample);
            auto outputSample = wetLevel * delayedSample;

            output[i] = outputSample;
        }
    }

    auto tuneCopy = std::make_unique<AudioBuffer<float>>();
    tuneCopy->makeCopyOf(buffer);
    
    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto reader = buffer.getWritePointer(ch);
        auto writer = tuneCopy->getWritePointer(ch);
        auto prevC = 0.0;

        for (size_t i = 0; i < numSamples; ++i)
        {
            auto prevOut = writer[i - 1];
            if (i == 0)
                prevOut = 0.0;

            float tuned = fineTune(reader[i], reader[i - 1], prevOut);
            writer[i] = tuned;
        }
    }

    buffer.makeCopyOf(*tuneCopy);
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
    jassert(newValue >= float(0)); //&& newValue <= float(1));
    feedback = newValue;
}

void Delay::setWetLevel(float newValue) noexcept
{
    jassert(newValue >= float(0) && newValue <= float(1));
    wetLevel = newValue;
}

void Delay::setDelayTime(size_t channel, int newValueSamples)
{
    if (channel >= getNumChannels())
    {
        jassertfalse;
        return;
    }

    jassert(newValueSamples >= 0);
    delayTimes[channel] = newValueSamples;

    updateDelayTime();
}

void Delay::setFundamentalFrequency(double f0)
{
    this->fundamentalFrequency = f0;
}

void Delay::setSampleRate(double sr)
{
    this->sampleRate = sr;
}

void Delay::updateDelayLineSize() {
    auto delayLineSizeSamples = (size_t)std::ceil(maxDelayTime * sampleRate);

    for (auto& dline : delayLines)
        dline.resize(delayLineSizeSamples);
}

void Delay::updateDelayTime() noexcept {
    for (size_t ch = 0; ch < maxNumChannels; ++ch) {
        //delayTimesSample[ch] = (size_t)juce::roundToInt(delayTimes[ch] * sampleRate);
        delayTimesSample[ch] = delayTimesSample[ch];
    }
}

/*
    Allpass filter that takes the current input (x_n), 
    the previous input (x_n-1) and the previous output (y_n-1) to produce correct tuning
*/
float Delay::fineTune(float input, float prevInput, float prevOutput)
{
    double phaseResponseForFrequency = filterCoefs->getPhaseForFrequency(fundamentalFrequency, sampleRate);
    float phaseDelay = static_cast<float>(-(phaseResponseForFrequency / fundamentalFrequency)) * sampleRate;
    float epsilon = 0.01;
    auto delayTimeSamples = delayTimes[0];

    auto samplePeriod = 1.0 / sampleRate;

    // Calculate P1
    auto P1 = sampleRate / fundamentalFrequency;

    float N = floorf(P1 - phaseDelay - epsilon);
    // Test, TODO maybe change the 0.5 part
    auto Pc = P1 - N - phaseDelay;

    // Calculate omega
    float omega = 2.0 * MathConstants<float>::pi * fundamentalFrequency;

    // Calculate C
    float C = sin( (omega * samplePeriod - omega * samplePeriod * Pc) / 2.0 )
        / sin( (omega * samplePeriod + omega * samplePeriod * Pc) / 2.0 );

    // Sample output
    float out = C * input + prevInput - C * prevOutput;
    
    return out;
}

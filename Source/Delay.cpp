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
    tuningFilterCoefs = new juce::dsp::IIR::Coefficients<float>(1.0f, C, C, 1.0);

    for (auto& f : filters)
    {
        f.prepare(spec);
        f.coefficients = filterCoefs;
    }

    for (auto& f : tuningFilters)
        f.prepare(spec);
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
        auto& tuningFilter = tuningFilters[ch];

        for (size_t i = 0; i < numSamples; ++i)
        {
            // Input from delay line
            auto delayedSample = filter.processSample(dline.get(delayTime));

            // Fine-tune the string using an allpass filter as described in the KS-extension paper
            float tuned = filter.processSample(delayedSample);

            // Input from main buffer
            auto inputSample = input[i];
            auto dlineInputSample = inputSample + feedback * tuned;
            // Hard-limit the amplitude output
            if (dlineInputSample < -1.0f)
                dlineInputSample = -1.0f;
            if (dlineInputSample > 1.0f)
                dlineInputSample = 1.0f;
            dline.push(dlineInputSample);
            auto outputSample = wetLevel * delayedSample;
            output[i] = outputSample;
        }
    }
}

void Delay::reset() noexcept
{
    for (auto& f : filters)
        f.reset();

    for (auto& f : tuningFilters)
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
    delayTimesSample[channel] = newValueSamples;

    //updateDelayTime();
}

void Delay::prepareFineTune(double fundamentalFrequency)
{
    double phaseResponseLowpass = filterCoefs->getPhaseForFrequency(fundamentalFrequency, sampleRate);
    float epsilon = 1.0 / sampleRate;
    double samplePeriod = 1.0 / sampleRate;

    double phaseDelayLowpass = -phaseResponseLowpass / (2.0 * MathConstants<double>::pi * fundamentalFrequency * samplePeriod);

    // Calculate P1
    auto P1 = sampleRate / fundamentalFrequency;

    int N = int(floor(P1 - phaseDelayLowpass - epsilon));
    setDelayTime(0, N);
    setDelayTime(1, N);

    double phaseDelayAllpass = P1 - N - phaseDelayLowpass;

    // Calculate omega
    double omega = MathConstants<double>::twoPi * fundamentalFrequency;

    // Calculate C
    double left = omega * samplePeriod;
    double right = omega * samplePeriod * phaseDelayAllpass;
    double dC = sin((left - right) / 2.0)
        / sin((left + right) / 2.0);

    C = static_cast<float>(dC);

    // Allpass filter for tuning
    

    tuningFilterCoefs = new juce::dsp::IIR::Coefficients<float>(1.0f, C, C, 1.0);
    for (auto& f : tuningFilters)
        f.coefficients = tuningFilterCoefs;

    auto p1check = N + phaseDelayLowpass + phaseDelayAllpass;

    auto test = 2;
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
        delayTimesSample[ch] = delayTimes[ch];
    }
}

/*
    Allpass filter that takes the current input (x_n), 
    the previous input (x_n-1) and the previous output (y_n-1) to produce correct tuning
*/
float Delay::fineTune(float input, float prevInput, float prevOutput)
{
    // Sample output
    float out = C * input + prevInput - C * prevOutput;
    return out;
}

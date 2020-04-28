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
    filterCoefs = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, float(sampleRate/2));
    //tuningFilterCoefs = new juce::dsp::IIR::Coefficients<float>(1.0f, C, C, 1.0);
    //tuningFilterCoefs = juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(sampleRate, int(sampleRate / 2));

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

    auto currentFeedback = feedback;
    if (isAdaptiveDecay) {
        currentFeedback = stretchFactor;
    }

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
            float tuned = tuningFilter.processSample(delayedSample);

            // Get input sample from main buffer
            auto inputSample = input[i];

            // Combine input and delayed samples
            auto dlineInputSample = inputSample + currentFeedback * tuned;
            dline.push(dlineInputSample);

            output[i] = delayedSample;
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

void Delay::windowChanged() {
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
}

void Delay::prepareFineTune(double fundamentalFrequency, int pitchBendRange, float pitchWheelValue)
{
    // Adjust f0 based on current pitch-wheel value
    auto semitoneRangeInCent = pitchBendRange * 100.0f;
    auto currentRange = pitchWheelValue * semitoneRangeInCent;
    
    // Get the current detune in cents for the incoming pitch bend value
    // "root" here comes from the Constants.h file and is the value any frequency must be multiplied
    // with to increase it by one cent (2^(1/1200)) in an equally tempered tuning
    double centMultiplicator = pow(root, currentRange);

    fundamentalFrequency *= centMultiplicator;

    auto pitchWheelMapped = pitchWheelValue * 10.0f;

    double phaseResponseLowpass = filterCoefs->getPhaseForFrequency(fundamentalFrequency, sampleRate);
    float epsilon = 0.01;
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

    C = dC;

    //auto test = -(1.0 / (left)) * atan(-sin(left) / (dC + cos(left)));

    // Set tuning-allpass filter coefficients based on incoming fundamental frequency
    tuningFilterCoefs = new juce::dsp::IIR::Coefficients<float>(1.0f, C, 1.0f, C);
    for (auto& f : tuningFilters)
        f.coefficients = tuningFilterCoefs;

    // If adaptive decay is enabled set stretch factor accordingly
    if (feedback == 1.0f)
        stretchFactor = 1.0f - log(1000) * (1.0f / fundamentalFrequency);
    else {
        auto factor = log(1000) * (1.0f / fundamentalFrequency);
        stretchFactor = feedback - log(1000) * (1.0f / fundamentalFrequency);
    }

    auto a = 2;
}

void Delay::setSampleRate(double sr)
{
    this->sampleRate = sr;
}

void Delay::setAdaptiveDecay(bool isAdaptiveDecay)
{
    this->isAdaptiveDecay = isAdaptiveDecay;
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
    float out = C * input + prevInput - (C * prevOutput);
    return out;
}


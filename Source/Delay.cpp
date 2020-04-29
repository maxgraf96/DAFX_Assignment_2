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
    sampleRate = float(spec.sampleRate);
    updateDelayLineSize();
    updateDelayTime();

    // Use first-order attenuation of the signal in the delay loop
    filterCoefs = dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, float(sampleRate/2));

	// Prepare the filters using the host's configuration
	// Lowpass for attenuation
    for (auto& f : filters)
    {
        f.prepare(spec);
        f.coefficients = filterCoefs;
    }

	// Allpass for fine-tuning
    for (auto& f : tuningFilters)
        f.prepare(spec);
}

void Delay::process(AudioBuffer<float>& buffer) noexcept
{
	// Get audio data from buffer
	const auto inputBuffer = buffer.getArrayOfWritePointers();
	const auto numSamples = buffer.getNumSamples();
	const auto numChannels = buffer.getNumChannels();

	// Get current feedback
    auto currentFeedback = feedback;
	// If adaptive decay is enabled, apply stretch factor to feedback
    if (isAdaptiveDecay) {
        currentFeedback = stretchFactor;
    }

    for (size_t ch = 0; ch < numChannels; ++ch)
    {
    	// Get input and output for channel
	    const auto input = inputBuffer[ch];
	    const auto output = inputBuffer[ch];
    	// Get delay line for channel
        auto& dline = delayLines[ch];
    	// Get delay time
	    const auto delayTime = delayTimes[ch];
    	// Get filters for channel
        auto& filter = filters[ch];
        auto& tuningFilter = tuningFilters[ch];

        for (size_t i = 0; i < numSamples; ++i)
        {
            // Input from delay line
            const auto delayedSample = filter.processSample(dline.get(delayTime));

            // Fine-tune the string using an allpass filter as described in the KS-extension paper
            const float tuned = tuningFilter.processSample(delayedSample);
            //float tuned = delayedSample;

            // Get input sample from main buffer
            const auto inputSample = input[i];

            // Combine input and delayed samples
            const auto dlineInputSample = inputSample + currentFeedback * tuned;
            dline.push(dlineInputSample);

        	// Write delayed sample to output
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
	// On window changes the delay line needs to be cleared
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
    const auto semitoneRangeInCent = float(pitchBendRange) * 100.0f;
    const auto currentRange = pitchWheelValue * semitoneRangeInCent;
    
    // Get the current detune in cents for the incoming pitch bend value
    // "root" here comes from the Constants.h file and is the value any frequency must be multiplied
    // with to increase it by one cent (2^(1/1200)) in an equally tempered tuning
    const double centMultiplicator = pow(root, currentRange);

	// Adapt f0 if pitchwheel is active
    fundamentalFrequency *= centMultiplicator;

	// Fine-tuning as described in the Karplus-Strong extension paper
	// This uses an allpass filter with variable phase delay to account for
	// the limit imposed by using an integer amount of delay elements

	// Get phase response of the lowpass filter 
    const double phaseResponseLowpass = filterCoefs->getPhaseForFrequency(fundamentalFrequency, sampleRate);
    const double samplePeriod = 1.0 / sampleRate;

	// Calculate phase delay from phase response
    const double phaseDelayLowpass = -phaseResponseLowpass / (2.0 * MathConstants<double>::pi * fundamentalFrequency * samplePeriod);

    // Calculate P1
    const auto P1 = sampleRate / fundamentalFrequency;

	// Calculate new delay line length N and set
    const int N = int(floor(P1 - phaseDelayLowpass - epsilon));
    setDelayTime(0, N);
    setDelayTime(1, N);

	// Calculate the phase delay required by the allpass filter in order to tune to correct pitch
    const double phaseDelayAllpass = P1 - N - phaseDelayLowpass;

    // Calculate omega
    const double omega = MathConstants<double>::twoPi * fundamentalFrequency;

    // Calculate the tuning coefficient C
    const double left = omega * samplePeriod;
    const double right = omega * samplePeriod * phaseDelayAllpass;

    C = sin((left - right) / 2.0) / sin((left + right) / 2.0);

    // Set tuning-allpass filter coefficients based on incoming fundamental frequency
    tuningFilterCoefs = new dsp::IIR::Coefficients<float>(1.0f, C, 1.0f, C);
    for (auto& f : tuningFilters)
        f.coefficients = tuningFilterCoefs;

    // If adaptive decay is enabled set stretch factor accordingly
    stretchFactor = feedback - log(1000) * (1.0f / fundamentalFrequency);
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
	const auto delayLineSizeSamples = size_t(std::ceil(maxDelayTime * sampleRate));

    for (auto& dline : delayLines)
        dline.resize(delayLineSizeSamples);
}

void Delay::updateDelayTime() noexcept {
    for (size_t ch = 0; ch < maxNumChannels; ++ch) {
        delayTimesSample[ch] = delayTimes[ch];
    }
}


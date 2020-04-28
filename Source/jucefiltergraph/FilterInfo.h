/*
  ==============================================================================

    FilterInfo.h
    
    Sean Enderby

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "../PluginProcessor.h"
#include <complex>
#include <vector>

#ifndef double_E
#define double_E 2.71828183
#endif
//====================================================================================
enum FilterType
{
	LowPass,
	HighPass
};
    
enum BandType
{
    LowShelf,
    HighShelf,
    Peak
};

//==============================================================================
class FilterResponse
{
public:
    FilterResponse (double magnitudeInit, double phaseInit);
    ~FilterResponse();
    
    double magnitudeValue, phaseValue;
};

//============================================================================
class FilterInfo
{
public:    
    FilterInfo(double sampleRate, Dafx_assignment_2AudioProcessor& p);
    ~FilterInfo();
    
    void setSampleRate (double sampleRate);
    void setGain (double gain);
    void updateFilter(float cutoff, float q);
    
    FilterResponse getResponse (double inputFrequency) const;

    double getCutoff();
    double getQ();

    void setCutoff(double cutoff);
    void setQ(double Q);
private:
    double fs;

    Dafx_assignment_2AudioProcessor& processor;

    std::array<juce::dsp::IIR::Coefficients<float>::Ptr, 2> coeffs;
    double gainValue;
    double cutoff;
    double q;
};
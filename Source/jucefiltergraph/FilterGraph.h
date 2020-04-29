/*
  ==============================================================================

    FilterGraph.h
    
    Sean Enderby

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterInfo.h"
#include "../Utility.h"
#include "../PluginProcessor.h"

//==============================================================================
class FilterGraph    : public Component,
                       public SettableTooltipClient,
    private AudioProcessorValueTreeState::Listener, ChangeBroadcaster, ChangeListener
{
public:
    //FilterGraph (int numFilters, TooltipWindow& tooltip);
    FilterGraph(double sampleRate, Dafx_assignment_2AudioProcessor& p, TooltipWindow& tooltip);
    ~FilterGraph();
    
    enum TraceType
    {
        Magnitude,
        Phase
    };
    
    void paint (Graphics&);
    void resized();
    
    void setNumHorizontalLines (int newValue);
    void setFreqRange (float newLowFreq, float newHighFreq);
    
    void setFilterGain (int filterNum, double gain);
    void setFilter (int filterNum, double sampleRate, double frequency, FilterType filterType);
    void setEqBand (int filterNum, double sampleRate, double frequency, double Q, float gain, BandType eqType);
    void setCustom (int filterNum, double sampleRate, std::vector <double> numCoeffs, std::vector <double> denCoeffs);
    
    float xToFreq (float xPos);
    float freqToX (float freq);

    void setTraceColour (Colour newColour);
    
    float maxdB, maxPhas;
    Colour traceColour;
    TraceType traceType;
    
private:
    TooltipWindow& tooltip;
    int numHorizontalLines;
    float lowFreq, highFreq;   
    double fs;
    int numFilters;

    // Flag for checking if dragging
    bool isDragging = false;
    Point<int> lastMousePosRel;
    Point<int> lastMousePosAbs;
    void mouseMove (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;

    void updateFilters(float cutoff, float q);

    void renderTooltip(Point<int>& mousePosRel, Point<int>& mousePosAbs);
    AudioProcessorValueTreeState& vts;
    
    std::vector <FilterInfo> filterVector;
    
    Path gridPath, tracePath;

    void parameterChanged(const String& parameterID, float newValue) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterGraph)
};
/*
  ==============================================================================

    FilterGraph.cpp
    
    Sean Enderby

  ==============================================================================
*/

#include "FilterGraph.h"

//==============================================================================
//FilterGraph::FilterGraph (int numFiltersInit, TooltipWindow& tooltip)
//    :tooltip(tooltip)
//{    
//    numHorizontalLines = 7;
//    setSize (500, 300);
//    lowFreq = 20;
//    highFreq = 20000;
//    fs = 48000;
//    maxdB = 6;
//    maxPhas = 1;
//    numFilters = numFiltersInit;
//    traceColour = Colour (0xaa00ff00);
//    traceType = Magnitude;
//}

FilterGraph::FilterGraph(double sampleRate, Dafx_assignment_2AudioProcessor& p, TooltipWindow& tooltip)
    :tooltip(tooltip), vts(p.getVTS())
{
    filterVector.push_back(FilterInfo(sampleRate, p));
    filterVector[0].setSampleRate(sampleRate);

    numHorizontalLines = 7;
    lowFreq = 20;
    highFreq = 20000;
    fs = sampleRate;
    maxdB = 6;
    maxPhas = 1;
    numFilters = 1; // Hard limit to 1 filter for now
    traceColour = Colour(0xaa00ff00);
    traceType = Magnitude;

    vts.addParameterListener("mainFilterCutoff", this);
    vts.addParameterListener("mainFilterQ", this);

    fs = sampleRate;
    repaint();

    addChangeListener(this);
}

FilterGraph::~FilterGraph()
{
    vts.removeParameterListener("mainFilterCutoff", this);
    vts.removeParameterListener("mainFilterQ", this);
}

void FilterGraph::paint (Graphics& g)
{
    // get size info =======================================================================================
    float width = (float) getWidth();
    float height = (float) getHeight();
    
    // paint the display background ============================================================================
    g.setGradientFill (ColourGradient (Colour (0xff232338), width / 2, height / 2, Colour (0xff21222a), 2.5f, height / 2, true));
    g.fillRect (2.5f, 2.5f, width - 5, height - 5);
    
    // paint the only trace at the moment ======================================================================
    tracePath.clear();
    
    if (traceType == Magnitude)
    {
        float scaleFactor = (((height / 2) - (height - 5) / (numHorizontalLines + 1) - 2.5f) / maxdB);
    
        float traceMagnitude = (float) (filterVector [0].getResponse (lowFreq).magnitudeValue);
        
        for (int i = 1; i < numFilters; i++)
        {
            traceMagnitude *= (float) (filterVector [i].getResponse (lowFreq).magnitudeValue);
        }
        traceMagnitude = 20 * log10 (traceMagnitude);
    
        tracePath.startNewSubPath (2.5f, (height / 2) - (traceMagnitude * scaleFactor));
        
        for (float xPos = 3.5; xPos < (width - 2.5); xPos += 1)
        {
            float freq = xToFreq (xPos);
            
            traceMagnitude = (float) (filterVector [0].getResponse (freq).magnitudeValue);
            
            for (int i = 1; i < numFilters; i++)
            {
                traceMagnitude *= (float) (filterVector [i].getResponse (freq).magnitudeValue);
            }
            traceMagnitude = 20 * log10 (traceMagnitude);
        
            tracePath.lineTo (xPos, (height / 2) - (traceMagnitude * scaleFactor));
        }
    }
    
    if (traceType == Phase)
    {
        float scaleFactor = ((height / 2) - (height - 5) / (numHorizontalLines + 1) - 2.5f) / (float_Pi * maxPhas);
    
        float traceMagnitude = (float) (filterVector [0].getResponse (lowFreq).phaseValue);
        for (int i = 1; i < numFilters; i++)
        {
            traceMagnitude += (float) (filterVector [i].getResponse (lowFreq).phaseValue);
        }
        
        float prevPhase = traceMagnitude;
        float unwrapSummand = 0;
        
        tracePath.startNewSubPath (2.5f, (height / 2) - (traceMagnitude * scaleFactor));
        for (float xPos = 3.5; xPos < (width - 2.5); xPos += 1)
        {
            float freq = xToFreq (xPos);
            
            traceMagnitude = (float) (filterVector [0].getResponse (freq).phaseValue);
            
            for (int i = 1; i < numFilters; i++)
            {
                traceMagnitude += (float) (filterVector [i].getResponse (freq).phaseValue);
            }
        
            if (fabs (traceMagnitude - prevPhase) > 5)
            {
                unwrapSummand += float_Pi * 2;
            }
            
            prevPhase = traceMagnitude;
            
            tracePath.lineTo (xPos, (height / 2) - ((traceMagnitude + unwrapSummand) * scaleFactor));
        }
    }
    
    g.setColour (traceColour);
    g.strokePath (tracePath, PathStrokeType (3.0f));
    
    // paint the display grid lines ===============================================================================
    g.setColour (Colour (0xaaffffff));
    String axisLabel;
    if (traceType == Magnitude)  axisLabel = String (maxdB, 1) + "dB";
    else if (traceType == Phase) 
    {
        axisLabel = String (CharPointer_UTF8 ("\xcf\x80")) + "rad";
        axisLabel = (maxPhas == 1) ? axisLabel : String (maxPhas, 1) + axisLabel;
    }
    
    g.setFont (Font ("Open Sans", 12.0f, Font::plain));
    g.drawText (axisLabel, 6, (int) ((height - 5) / (numHorizontalLines + 1) -9.5f), 45, 12, Justification::left, false);
    g.drawText (String ("-") + axisLabel, 6, (int) (numHorizontalLines * (height - 5) / (numHorizontalLines + 1) + 3.5f), 45, 12, Justification::left, false);
    
    gridPath.clear();
    for (int lineNum = 1; lineNum < numHorizontalLines + 1; lineNum++)
    {
        float yPos = lineNum * (height - 5) / (numHorizontalLines + 1) + 2.5f;
        gridPath.startNewSubPath (2.5f, yPos);
        gridPath.lineTo (width - 2.5f, yPos);
    }
    
    float order = (pow (10, floor (log10 (lowFreq))));
    float rounded = order * (floor(lowFreq / order) + 1);
    for (float freq = rounded; freq < highFreq; freq += pow (10, floor (log10 (freq))))
    {
        float xPos = freqToX (freq);
        gridPath.startNewSubPath (xPos, 2.5f);
        gridPath.lineTo (xPos, height - 2.5f);
    }
    g.excludeClipRegion (Rectangle <int> (6, (int) ((height - 5) / (numHorizontalLines + 1) -9.5f), 45, 12));
    g.excludeClipRegion (Rectangle <int> (6, (int) (numHorizontalLines * (height - 5) / (numHorizontalLines + 1) + 3.5f), 45, 12));
    g.setColour (Colour (0x60ffffff));   
    g.strokePath (gridPath, PathStrokeType (1.0f));
    
    // draw the border ======================================================================================
    g.setColour (Colours::black);
    g.drawRect (2.5f, 2.5f, width - 5, height - 5, 1.000f);

    // Draw tooltip if dragging
    if (isDragging) {
        renderTooltip(lastMousePosRel, lastMousePosAbs);
    }
}

void FilterGraph::resized()
{
}

void FilterGraph::setNumHorizontalLines (int newValue)
{
    numHorizontalLines = newValue;
    repaint();
}

void FilterGraph::setFreqRange (float newLowFreq, float newHighFreq)
{
    lowFreq = fabs (newLowFreq + 0.1f);
    highFreq = fabs (newHighFreq);
    repaint();
}

void FilterGraph::setFilterGain (int filterNum, double gain)
{
    filterVector [filterNum].setGain (gain);
    repaint();
}

void FilterGraph::setFilter (int filterNum, double sampleRate, double frequency, FilterType filterType)
{    
    filterVector [filterNum].setSampleRate (sampleRate);
    //filterVector [filterNum].setFilter (frequency, filterType);
    
    fs = sampleRate;
    repaint();
}

void FilterGraph::setEqBand (int filterNum, double sampleRate, double frequency, double Q, float gain, BandType eqType)
{    
    filterVector [filterNum].setSampleRate (sampleRate);
    //filterVector [filterNum].setEqBand (frequency, Q, gain, eqType);
    
    fs = sampleRate;
    repaint();
}

void FilterGraph::setCustom (int filterNum, double sampleRate, std::vector <double> numCoeffs, std::vector <double> denCoeffs)
{
    filterVector [filterNum].setSampleRate (sampleRate);
    //filterVector [filterNum].setCustom (numCoeffs, denCoeffs);
    
    fs = sampleRate;
    repaint();
}

float FilterGraph::xToFreq (float xPos)
{
    float width = (float) getWidth();
    return lowFreq * pow ((highFreq / lowFreq), ((xPos - 2.5f) / (width - 5.0f)));
}

float FilterGraph::freqToX (float freq)
{
    float width = (float) getWidth();
    return (width - 5) * (log (freq / lowFreq) / log (highFreq / lowFreq)) + 2.5f;
}

void FilterGraph::setTraceColour (Colour newColour)
{
    traceColour = newColour;
    repaint();
}

void FilterGraph::mouseMove (const MouseEvent &event)
{    
    repaint();

    /*if (traceType == Phase)
    {
        float phase = (float) (filterVector [0].getResponse (freq).phaseValue);
    
        for (int i = 1; i < numFilters; i++)
        {
            phase += (float) (filterVector [i].getResponse (freq).phaseValue);
        }
        
        phase /= float_Pi;
    
        tooltip.displayTip (mousePos, String (freq, 1) + "Hz, " + String (phase, 2) + String (CharPointer_UTF8 ("\xcf\x80")) + "rad");
    }*/
}

void FilterGraph::mouseDrag(const MouseEvent& event)
{
    lastMousePosRel = getMouseXYRelative();
    lastMousePosAbs = event.getScreenPosition();
    
    // Get x position for cutoff frequency
    int xPos = lastMousePosRel.getX();
    if (xPos < 0) xPos = 0;
    if (xPos > getWidth()) xPos = getWidth();

    // Get y position for Q value
    int yPos = lastMousePosRel.getY();
    if (yPos < 0) yPos = 0;
    if (yPos > getHeight()) yPos = getHeight();

    // Convert from coordinates to values
    float freq = xToFreq(xPos);
    float q = map(yPos, 0.0f, static_cast<float>(getHeight()), 3.0f, 0.001f);

    // Update filter coefficients
    updateFilters(freq, q);

	// Asynchronous repaint
    sendChangeMessage();
}

void FilterGraph::mouseDown(const MouseEvent& event) {
    isDragging = true;
}

void FilterGraph::mouseUp(const MouseEvent& event) {
    // ...
    isDragging = false;
}

void FilterGraph::updateFilters(float cutoff, float q) {
    // Set cutoff frequency
    vts.getParameterAsValue("mainFilterCutoff").setValue(cutoff);
    vts.getParameterAsValue("mainFilterQ").setValue(q);

    filterVector[0].updateFilter(cutoff, q);
}

void FilterGraph::renderTooltip(Point<int>& mousePosRel, Point<int>& mousePosAbs) {
    int xPos = lastMousePosRel.getX();
    if (xPos < 0)
        xPos = 0;
    if (xPos > getWidth())
        xPos = getWidth();

    float freq = xToFreq(xPos);

    // Display tooltip
    if (traceType == Magnitude)
    {
        float magnitude = (float)(filterVector[0].getResponse(freq).magnitudeValue);

        magnitude = 20 * log10(magnitude);

        // Display tooltip
        tooltip.displayTip(mousePosAbs, String(freq, 1) + "Hz, " + String(magnitude, 1) + "dB");
    }
}

void FilterGraph::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == "mainFilterCutoff") {
        filterVector[0].setCutoff(newValue);
        sendChangeMessage();
    }
    if (parameterID == "mainFilterQ") {
        filterVector[0].setQ(newValue);
        sendChangeMessage();
    }
}

void FilterGraph::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == this) 
        repaint();
}

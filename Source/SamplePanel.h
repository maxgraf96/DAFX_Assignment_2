/*
  ==============================================================================

    SamplePanel.h
    Created: 15 Mar 2020 3:40:25pm
    Author:  Music

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utility.h"
#include "Constants.h"

//==============================================================================
/*
*/
class SamplePanel    : public Component, FilenameComponentListener
{
public:
    SamplePanel(int windowLength);
    ~SamplePanel();

    void paint (Graphics&) override;
    void resized() override;
    // For access in PluginProcessor
    AudioBuffer<float>* getSampleBuffer();
    // Returns current position in sample in seconds
    double getSamplePosition();
    void setSampleRate(double sampleRate);
    void setWindowLength(float windowLength);

private:
    double sampleRate = 0.0;
    // Window length
    int windowLength = 0;

    // For detecting mouse clicks
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseEventUpdateSamplePosition(const MouseEvent& event);
    NormalisableRange<int> normaliser;

    // State for file loaded
    enum FileLoadedState { notLoaded, loading, loaded, rejected };
    FileLoadedState fileLoadedState;

    // Component for loading files
    std::unique_ptr<FilenameComponent> filenameComponent;

    // Pointer to AudioBuffer in procesor that holds the samples once a file is loaded
    std::unique_ptr<AudioBuffer<float>> sampleBuffer;

    // For displaying the waveform
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    void paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);

    void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;
    void loadFile(File file);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplePanel)
};

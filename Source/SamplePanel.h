/*
  ==============================================================================

    SamplePanel.h
    Created: 15 Mar 2020 3:40:25pm
    Author:  Music

  ==============================================================================
*/

#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Utility.h"
#include "Constants.h"

using namespace juce;
//==============================================================================
/*
*/
class SamplePanel    : public Component, FilenameComponentListener, private Value::Listener, ChangeListener
{
public:
    SamplePanel(AudioProcessorValueTreeState& vts);
    ~SamplePanel();

    void paint (Graphics&) override;
    void resized() override;
    // For access in PluginProcessor
    AudioBuffer<float>* getSampleBuffer() const;
	// Set the window position using a value between [0...1]
	// which is then mapped to the sample length
    void setSamplePosition(float position);
	// Helper method to set the sample position after the state is loaded
    void setSamplePositionFromState(float position);
    // Returns current position in sample in seconds
    double getSamplePosition();
	// Set the sample rate
    void setSampleRate(double sampleRate);
	// Set the length of the window
    void setWindowLength(float windowLength);
	// Set the file path to current sample
    void setCurrentFilePath(String& path);

	// ID of the currentFilePath value
	// This is needed to load a file after the plugin's state is initialised
	// i.e. if this particular instance of the plugin was used e.g. in a DAW and is then reloaded
    static Identifier currentFilePathID;
    Value currentFilePath;
private:
    // State management
    AudioProcessorValueTreeState& valueTreeState;
    // Helper used to load the window position after state initialisation
    float positionFromState = 0.0f;
    // Current sample rate
    double sampleRate = 0.0;
    // Window length
    int windowLength = 0;

    // For handling mouse events
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseEventUpdateSamplePosition(const MouseEvent& event);

    // State for file loaded
    enum FileLoadedState { notLoaded, loading, loaded, rejected };
    FileLoadedState fileLoadedState;

    // Component for loading files
    std::unique_ptr<FilenameComponent> filenameComponent;

    // Pointer to AudioBuffer in procesor that holds the samples once a file is loaded
    std::unique_ptr<AudioBuffer<float>> sampleBuffer;

    // Waveform display related fields
	// Height of the waveform-display rectangle
	const int WAVEFORM_HEIGHT = 200;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

	// Painting directive if no sample is loaded
    void paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
	// Painting directive if a sample is loaded
    void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);

	// Callback that's triggered if a new file is loaded
    void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;
	// Load a file to the sample buffer
    void loadFile(const File& file);

	// Callback currently only to listen to file-path changes
    void valueChanged(Value& val) override;

    // Listen to changes on the thumbnail to repaint when necessary
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void transportSourceChanged();
    void thumbnailChanged();

	// Colours
	Colour backgroundColour = Colour(0xff220901);
	Colour timeMarkerColour = Colour(0xff5EB0A7);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplePanel)
};
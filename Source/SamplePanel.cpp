/*
  ==============================================================================

    SamplePanel.cpp
    Created: 15 Mar 2020 3:40:25pm
    Author:  Music

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SamplePanel.h"

//==============================================================================
SamplePanel::SamplePanel(Dafx_assignment_2AudioProcessor& p)
    : processor(p), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache)
{
    // Initialise sample buffer
    sampleBuffer.reset(new AudioBuffer<float>());

    // Initialise loaded state
    fileLoadedState = notLoaded;

    filenameComponent.reset(
        new FilenameComponent("file", {}, false, false, false, "*.wav", {}, "Browse for sample")
    );
    // Set default folder for convenience for now
    auto defaultFolder = new File("C:\\Users\\Music\\Ableton\\Mixes\\eam");
    filenameComponent->setDefaultBrowseTarget(*defaultFolder);
    
    // Add file picker component to UI
    addAndMakeVisible(filenameComponent.get());
    filenameComponent->addListener(this);

    // Add waveform display
    formatManager.registerBasicFormats();
    //thumbnail.addChangeListener(this);

    // Set component size
    setSize(1024, 200);
}

SamplePanel::~SamplePanel()
{
}

void SamplePanel::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);

    String fileLoadedText = "";
    if (fileLoadedState == notLoaded) {
        fileLoadedText = "No sample loaded";
    }
    else if (fileLoadedState == loading) {
        fileLoadedText = "Drop sample here";
    }
    else if (fileLoadedState == rejected) {
        fileLoadedText = "Only '.wav' files are supported!";
    }
    else {
        fileLoadedText = "Loaded";
    }
    g.drawText (fileLoadedText, getLocalBounds(),
                Justification::centred, true);   

    // Draw waveform if available
    Rectangle<int> thumbnailBounds(0, 12, 1024, 200);
    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void SamplePanel::paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour(Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
}

void SamplePanel::paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    // Draw background
    g.setColour(Colours::darkgrey);
    g.fillRect(thumbnailBounds);

    // Draw waveform
    g.setColour(Colours::lightgrey);
    thumbnail.drawChannels(g,
        thumbnailBounds,
        0.0,                                    // start time
        thumbnail.getTotalLength(),             // end time
        1.0f);                                  // vertical zoom
}

void SamplePanel::resized()
{
    filenameComponent->setBounds(0, 0, 1024, 30);
}

/*
    Loads an audio file into the application and stores it in the sample buffer
*/
void SamplePanel::loadFile(File file) {
    
    auto fileExtension = file.getFileExtension();

    // Only accept wavs
    if (fileExtension == ".wav" || fileExtension == ".WAV") {
        ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
        // If file is valid load it and send callback to processor
        if (reader != 0)
        {
            // Read into sample buffer
            sampleBuffer->setSize(reader->numChannels, reader->lengthInSamples);
            reader->read(sampleBuffer.get(), 0, reader->lengthInSamples, 0, true, true);

            // Display thumbnail
            readerSource.reset(new AudioFormatReaderSource(reader, true));
            thumbnail.setSource(new FileInputSource(file));

            // Send callback to processor
            processor.sampleLoadedCallback(sampleBuffer.get());
            
            // Reset / release resources
            readerSource.reset();
            reader.release();
        }

        fileLoadedState = loaded;
        repaint();
    }
    else {
        // Reset file component
        filenameComponent->setCurrentFile({}, false, juce::NotificationType::dontSendNotification);
        // Show info text
        fileLoadedState = rejected;
        repaint();
    }
}

void SamplePanel::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) 
{
    if (fileComponentThatHasChanged == filenameComponent.get())
        loadFile(filenameComponent->getCurrentFile());
}
/*
  ==============================================================================

    SamplePanel.cpp
    Created: 15 Mar 2020 3:40:25pm
    Author:  Music

  ==============================================================================
*/

#include "SamplePanel.h"

Identifier SamplePanel::currentFilePathID("currentFilePath");

//==============================================================================
SamplePanel::SamplePanel(int windowLength, AudioProcessorValueTreeState& vts)
    : thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache), valueTreeState(vts)
{
    this->windowLength = windowLength;

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

    // State management
    currentFilePath.addListener(this);
    //currentFilePath.referTo(valueTreeState.state.getPropertyAsValue(currentFilePathID, nullptr));

    // Preload sample for convenience
    auto currentFile = new File("");
    filenameComponent->setCurrentFile(*currentFile, false, dontSendNotification);
    loadFile(*currentFile);

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
    auto audioLength(thumbnail.getTotalLength());
    g.setColour(Colours::lightgrey);
    thumbnail.drawChannels(g,
        thumbnailBounds,
        0.0,                                    // start time
        audioLength,             // end time
        1.0f);                                  // vertical zoom

    // Draw time marker
    g.setColour(Colours::green);

    auto audioPosition(transportSource.getCurrentPosition());
    auto drawPosition((audioPosition / audioLength) * thumbnailBounds.getWidth()
        + thumbnailBounds.getX());
    g.drawLine(drawPosition, thumbnailBounds.getY(), drawPosition,
        thumbnailBounds.getBottom(), 2.0f);
    // Draw window length lines
    auto startPosition = ((audioPosition - static_cast<double>(windowLength) / 2.0 / sampleRate) / audioLength)* thumbnailBounds.getWidth();
    auto endPosition = ((audioPosition + static_cast<double>(windowLength) / 2.0 / sampleRate) / audioLength)* thumbnailBounds.getWidth();
    g.setColour(Colours::lightgrey);
    g.drawLine(startPosition, thumbnailBounds.getY(), startPosition,
        thumbnailBounds.getBottom(), 1.0f);
    g.drawLine(endPosition, thumbnailBounds.getY(), endPosition,
        thumbnailBounds.getBottom(), 1.0f);
}

void SamplePanel::resized()
{
    filenameComponent->setBounds(0, 0, 1024, 30);
}

AudioBuffer<float>* SamplePanel::getSampleBuffer()
{
    return sampleBuffer.get();
}

void SamplePanel::setSamplePosition(float position)
{
    auto fileLengthInSeconds = float(transportSource.getLengthInSeconds());
    // Map to sample 
    double mapped = map(position, 0.0f, 1.0, 0.0, fileLengthInSeconds);
    // Constrain to keep from going out of component bounds
    if (mapped < 0.0f)
        mapped = 0.0f;
    if (mapped > fileLengthInSeconds)
        mapped = fileLengthInSeconds;

    // Set marker
    transportSource.setPosition(mapped);
    repaint();
}

double SamplePanel::getSamplePosition()
{
    return transportSource.getCurrentPosition();
}

void SamplePanel::setSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void SamplePanel::mouseDown(const MouseEvent& mouseEvent)
{
    mouseEventUpdateSamplePosition(mouseEvent);
}

void SamplePanel::mouseDrag(const MouseEvent& mouseEvent)
{
    mouseEventUpdateSamplePosition(mouseEvent);
}

void SamplePanel::mouseEventUpdateSamplePosition(const MouseEvent& mouseEvent) {
    // Get x position in terms of component
    auto xPos = mouseEvent.getPosition().getX();
    auto fileLengthInSeconds = float(transportSource.getLengthInSeconds());
    // Map to sample 
    double mapped = map(float(xPos), 0.0f, float(getWidth()), 0.0, fileLengthInSeconds);
    // Constrain to keep from going out of component bounds
    if (mapped < 0.0f)
        mapped = 0.0f;
    if (mapped > fileLengthInSeconds)
        mapped = fileLengthInSeconds;

    // Set marker
    transportSource.setPosition(mapped);

    // Set parameter as well to keep in sync
     // Map to [0...1]
    mapped = map(float(xPos), 0.0f, float(getWidth()), 0.0, 1.0);
    valueTreeState.getParameterAsValue("position").setValue(mapped);

    repaint();
}

/*
    Loads an audio file into the application and stores it in the sample buffer
*/
void SamplePanel::loadFile(File file) {
    
    auto fileExtension = file.getFileExtension();

    // Only accept wavs
    if (fileExtension == ".wav" || fileExtension == ".WAV") {
        filenameComponent->setCurrentFile(file, false, dontSendNotification);
        ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
        // If file is valid load it and send callback to processor
        if (reader != 0)
        {
            // Read into sample buffer
            sampleBuffer->setSize(reader->numChannels, reader->lengthInSamples);
            reader->read(sampleBuffer.get(), 0, reader->lengthInSamples, 0, true, true);

            // Display thumbnail
            std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            thumbnail.setSource(new FileInputSource(file));
            
            // Reset / release resources
            readerSource.reset(newSource.release());
            reader.release();
        }

        fileLoadedState = loaded;
        repaint();
    }
    else {
        // Reset file component
        filenameComponent->setCurrentFile({}, false, dontSendNotification);
        // Show info text
        fileLoadedState = rejected;
        repaint();
    }
}

void SamplePanel::valueChanged(Value& val)
{
    if (val.toString() != "") {
        valueTreeState.state.setProperty(currentFilePathID, val.toString(), nullptr);
        auto currentFile = new File(val.toString());
        loadFile(*currentFile);
    }
}

void SamplePanel::setWindowLength(float windowLength) {
    auto l = map(windowLength, 0.0, 1.0, WINDOW_LENGTH_MIN, WINDOW_LENGTH_MAX);
    this->windowLength = static_cast<int>(l);
    repaint();
}

void SamplePanel::setCurrentFilePath(String& path)
{
    currentFilePath.setValue(path);
}

void SamplePanel::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) 
{
    if (fileComponentThatHasChanged == filenameComponent.get()) {
        currentFilePath.setValue(filenameComponent->getCurrentFile().getFullPathName());
    }
}
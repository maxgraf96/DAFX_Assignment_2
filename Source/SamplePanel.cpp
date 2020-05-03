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
SamplePanel::SamplePanel(AudioProcessorValueTreeState& vts)
    : thumbnailCache(5), thumbnail(1024, formatManager, thumbnailCache), valueTreeState(vts)
{
	// Get window length from state management
    this->windowLength = static_cast<int>(*vts.getRawParameterValue("windowLength"));

    // Initialise sample buffer
    sampleBuffer.reset(new AudioBuffer<float>());

    // Initialise loaded state
    fileLoadedState = notLoaded;

    filenameComponent.reset(
        new FilenameComponent("file", {}, false, false, false, "*.wav", {}, "Browse for sample")
    );
    
    // Add file picker component to UI
    addAndMakeVisible(filenameComponent.get());
    filenameComponent->addListener(this);

    // Prepare waveform display
    formatManager.registerBasicFormats();

    // Hook up currentFilePath to state management
    currentFilePath.addListener(this);

    // Preload sample for convenience
	// Note: If this path does not exist the plugin simply defaults to no sample
    // const auto currentFile = new File("C:\\Users\\Music\\Ableton\\Mixes\\sweetrelease.wav");
    // filenameComponent->setCurrentFile(*currentFile, false, dontSendNotification);
    // loadFile(*currentFile);

    // Listen to thumbnail changes
    transportSource.addChangeListener(this);
    thumbnail.addChangeListener(this);

    // Set component size
    setSize(1024, 200);
}

SamplePanel::~SamplePanel()
{
}

void SamplePanel::paint (Graphics& g)
{
    g.fillAll (backgroundColour);

    //g.setColour (Colours::grey);
    //g.drawRect (getLocalBounds(), 1);

    g.setColour (Colours::white);
    g.setFont(24.0f);

	// Text to display on sample panel depends on whether a file is loaded
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
    const Rectangle<int> thumbnailBounds(0, 12, getWidth(), WAVEFORM_HEIGHT);
    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void SamplePanel::paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour(backgroundColour);
    g.fillRect(thumbnailBounds);
    g.setColour(Colours::white);
	g.setFont(24.0f);
    g.drawFittedText("Click to load sample", thumbnailBounds, Justification::centred, 1.0f);
}

void SamplePanel::paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    // Draw background
    g.setColour(backgroundColour);
    g.fillRect(thumbnailBounds);

    // Draw waveform
    auto audioLength(thumbnail.getTotalLength());
    g.setColour(Colours::lightgrey);
    thumbnail.drawChannels(g,
        thumbnailBounds,
        0.0,                        // start time
        audioLength,                // end time
        0.9f);                      // vertical zoom
    // Draw time marker
    g.setColour(timeMarkerColour);

    const auto audioPosition(transportSource.getCurrentPosition());
    const auto drawPosition((audioPosition / audioLength) * thumbnailBounds.getWidth()
        + thumbnailBounds.getX());
    g.drawLine(drawPosition, thumbnailBounds.getY(), drawPosition,
        thumbnailBounds.getBottom(), 2.0f);
    // Draw window length lines
    const auto startPosition = ((audioPosition - static_cast<double>(windowLength) / 2.0 / sampleRate) / audioLength)* thumbnailBounds.getWidth();
    const auto endPosition = ((audioPosition + static_cast<double>(windowLength) / 2.0 / sampleRate) / audioLength)* thumbnailBounds.getWidth();
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

AudioBuffer<float>* SamplePanel::getSampleBuffer() const
{
    return sampleBuffer.get();
}

void SamplePanel::setSamplePosition(float position)
{
	const auto fileLengthInSeconds = float(transportSource.getLengthInSeconds());
    // Map to sample 
    double mapped = map(position, 0.0f, 1.0f, 0.0f, fileLengthInSeconds);
    // Constrain to keep from going out of component bounds
    if (mapped < 0.0f)
        mapped = 0.0f;
    if (mapped > fileLengthInSeconds)
        mapped = fileLengthInSeconds;

    // Set marker
    transportSource.setPosition(mapped);
    // Thread-safe repaint
    transportSource.sendChangeMessage();
}



void SamplePanel::mouseEventUpdateSamplePosition(const MouseEvent& mouseEvent) {
    // If no sample is loaded open the file-chooser window
    if (fileLoadedState != loaded) {
        FileChooser myChooser("Please select the file you want to load...",
            File::getSpecialLocation(File::userHomeDirectory),
            "*.wav"); // Only *.wav files are accepted at the moment

        if (myChooser.browseForFileToOpen())
        {
	        const File file(myChooser.getResult());
            currentFilePath.setValue(file.getFullPathName());
        }
    } else
    {
	    // Get x position in terms of component
	    const auto xPos = mouseEvent.position.getX();
	    const auto fileLengthInSeconds = float(transportSource.getLengthInSeconds());
	    // Map to sample 
	    double mapped = map(xPos, 0.0f, static_cast<float>(getWidth()), 0.0f, fileLengthInSeconds);
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
}

void SamplePanel::loadFile(const File& file) {
	// Get file extension
	const auto fileExtension = file.getFileExtension();
    // Only accept wavs
    if (fileExtension == ".wav" || fileExtension == ".WAV") {
        filenameComponent->setCurrentFile(file, false, dontSendNotification);
        ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
        // If file is valid load it and send callback to processor
        if (reader != 0)
        {
        	// Fix channels to 2 (stereo)
            // Read into sample buffer
            const auto numChannels = 2;
            sampleBuffer->setSize(numChannels, reader->lengthInSamples);
			reader->read(sampleBuffer.get(), 0, reader->lengthInSamples, 0, true, true);
            // If the incoming sound is mono, use the same data for both channels
        	if (reader->numChannels == 1)
        	{
        		const auto leftChannelReader = sampleBuffer->getReadPointer(0);
        		const auto rightChannelWriter = sampleBuffer->getWritePointer(1);
        		for (int sample = 0; sample < sampleBuffer->getNumSamples(); sample++)
        		{
        			rightChannelWriter[sample] = leftChannelReader[sample];
        		}
        	}

            // Display thumbnail
            std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            thumbnail.setSource(new FileInputSource(file));
            
            // Reset / release resources
            readerSource.reset(newSource.release());
            reader.release();

        	// Restore position if loaded
	        if (positionFromState > 0.0) {
	            setSamplePosition(positionFromState);
	            positionFromState = 0.0;
	        }

        	// Set file loaded state
	        fileLoadedState = loaded;
	        repaint();
	    }
    }
    else {
        // Reset file component
        filenameComponent->setCurrentFile({}, false, dontSendNotification);
        // Show info text
        fileLoadedState = rejected;
        repaint();
    }
}

// ----------------- Listeners ------------------------
void SamplePanel::valueChanged(Value& val)
{
    if (val.toString() != "") {
        valueTreeState.state.setProperty(currentFilePathID, val.toString(), nullptr);
    	// Create JUCE file
        const auto currentFile = new File(val.toString());
        // Load file into sample buffer
    	loadFile(*currentFile);
    }
}

void SamplePanel::changeListenerCallback(ChangeBroadcaster* source)
{
	// Currently those callbacks only repaint on changes made to the UI
    if (source == &transportSource) transportSourceChanged();
    if (source == &thumbnail) thumbnailChanged();
}

void SamplePanel::transportSourceChanged()
{
    repaint();
}

void SamplePanel::thumbnailChanged()
{
    repaint();
}

void SamplePanel::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) 
{
	// Update the currentFilePath value if the file was changed
    if (fileComponentThatHasChanged == filenameComponent.get()) {
        currentFilePath.setValue(filenameComponent->getCurrentFile().getFullPathName());
    }
}

// ----------------- Getters and setters ------------------------
void SamplePanel::setWindowLength(float windowLength) {
    this->windowLength = static_cast<int>(windowLength);

    // Thread-safe repaint
    transportSource.sendChangeMessage();
}

void SamplePanel::setCurrentFilePath(String& path)
{
    currentFilePath.setValue(path);
}

void SamplePanel::setSamplePositionFromState(float position)
{
    positionFromState = position;
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
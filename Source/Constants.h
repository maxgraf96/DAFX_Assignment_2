/*
  ==============================================================================

    Constants.h
    Created: 26 Mar 2020 5:01:33pm
    Author:  Music

  ==============================================================================
*/

#pragma once
#ifndef CONSTANTS_H
#include <juce_audio_processors/juce_audio_processors.h>
#endif

using namespace juce;

// The program-wide signifier for "not playing"
// This can be applied to buffers, MIDI states etc.
const int NOT_PLAYING = -1;

// Minimum and maximum allowed lengths for the window that is used to extract the excitator burst from the loaded sample
const int WINDOW_LENGTH_MIN = 1200;
const int WINDOW_LENGTH_MAX = 95999;

// Maximum number of allowed voices in the synthesis process
const int NUM_VOICES = 16;

// Maximum value for pitch wheel data (14 bit)
// From JUCE docs: "The value returned is a 14-bit number from 0 to 0x3fff, indicating the wheel position."
const float MIDI_PITCH_WHEEL_MAX_VAL = 16384.0f;

// The root for an octave (1200 semitones) used to calculate f0 frequency values for pitch shifting
const double root = std::exp(std::log(2) / 1200);

// Adapted ranges for Sliders. Those use skewFactors != 1 in order to give more/less room for a certain range of values
// E.g. For a slider used to control a lowpass filter cutoff it makes sense to reserve more screen-space for the lower frequency bands
const NormalisableRange<float> adsrRange(0, 3000, 1, 0.3f, false);
const NormalisableRange<float> delayFeedbackRange(0.80, 1.0f, 0.001f, 2.0f, false);
const NormalisableRange<float> mainFilterCutoffRange(1.0, 20000.0f, 1.0f, 0.3f, false);
const NormalisableRange<float> windowLengthRange(float(WINDOW_LENGTH_MIN), float(WINDOW_LENGTH_MAX), 1.0f, 0.3f, false);
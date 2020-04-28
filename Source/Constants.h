/*
  ==============================================================================

    Constants.h
    Created: 26 Mar 2020 5:01:33pm
    Author:  Music

  ==============================================================================
*/

#pragma once
const int WINDOW_LENGTH_MIN = 1200; // 5ms if sample rate is 48kHz
const int WINDOW_LENGTH_MAX = 20000;
const int NUM_VOICES = 16;
const int NOT_PLAYING = -1;
const float MIDI_PITCH_WHEEL_MAX_VAL = 16384.0f;
const double root = std::exp(std::log(2) / 1200);
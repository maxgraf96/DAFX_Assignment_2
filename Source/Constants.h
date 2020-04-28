/*
  ==============================================================================

    Constants.h
    Created: 26 Mar 2020 5:01:33pm
    Author:  Music

  ==============================================================================
*/

#pragma once
const int WINDOW_LENGTH_MIN = 2400; // 5ms if sample rate is 48kHz
const int WINDOW_LENGTH_MAX = 20000;
const int NUM_VOICES = 16;
const double root = std::exp(std::log(2) / 1200);
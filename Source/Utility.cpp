/*
  ==============================================================================

    Utility.cpp
    Created: 26 Mar 2020 5:09:41pm
    Author:  Music

  ==============================================================================
*/

#include "Utility.h"
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
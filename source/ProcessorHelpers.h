/*
  ==============================================================================

    ProcessorHelpers.h
    Created: 19 Jul 2020 3:05:37pm
    Author:  Aidan

	A collection of helper functions used within the context of the DSP
	of the program, all kept in one place for organizational reasons.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// take in the point position and spit out a "heading" angle where 0 degrees = up, 90 = right, 180 = down, 270 = left
inline float angleCalculator(juce::Point<float> point)
{
	float radians = atanf(point.getX() / point.getY()); // do the arctan function
	float degrees = abs(radians * (180 / MathConstants<float>::pi)); // convert to degrees

	//if (point.getX() > 0.0f && point.getY() > 0.0f) // top right quadrant
	//	dont need to do anything for this one
	if (point.getX() > 0.0f && point.getY() < 0.0f) // bottom right quadrant
		degrees = jmap(degrees, 90.0f, 0.0f, 90.0f, 180.0f);
	if (point.getX() < 0.0f && point.getY() < 0.0f) // bottom left quadrant
		degrees = jmap(degrees, 0.0f, 90.0f, 180.0f, 270.0f);
	if (point.getX() < 0.0f && point.getY() > 0.0f) // top left quadrant
		degrees = jmap(degrees, 90.0f, 0.0f, 270.0f, 360.0f);

	// safety bits

	if (point.getX() == 0 && point.getY() == 0)
		degrees = 0;

	return degrees;
}
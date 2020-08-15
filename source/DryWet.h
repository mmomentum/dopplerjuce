/*
  ==============================================================================

    DryWet.h
    Created: 13 Aug 2020 1:35:46pm
    Author:  Aidan

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

/** simple processor to mix dry and wet signals */
class DryWetProcessor
{
public:
	DryWetProcessor() {}

	void setDryWet(float newDryWet) { dryWet = newDryWet; }
	float getDryWet() const noexcept { return dryWet; }

	void reset() { lastDryWet = dryWet; }

	/**
	  mix dry and wet buffer. mixed buffer returned in place of wet buffer
	*/
	void processBlock(AudioBuffer<float>& dryBuffer, AudioBuffer<float>& wetBuffer)
	{
		if (lastDryWet == dryWet)
		{
			wetBuffer.applyGain(dryWet);
			for (int ch = 0; ch < wetBuffer.getNumChannels(); ++ch)
				wetBuffer.addFrom(ch, 0, dryBuffer.getReadPointer(ch), wetBuffer.getNumSamples(), (1.0f - dryWet));
		}
		else
		{
			for (int ch = 0; ch < wetBuffer.getNumChannels(); ++ch)
			{
				wetBuffer.applyGainRamp(ch, 0, wetBuffer.getNumSamples(), lastDryWet, dryWet);
				wetBuffer.addFromWithRamp(ch, 0, dryBuffer.getReadPointer(ch), wetBuffer.getNumSamples(), (1.0f - lastDryWet), (1.0f - dryWet));
			}

			lastDryWet = dryWet;
		}
	}

private:
	float dryWet = 0.0f;
	float lastDryWet = 0.0f;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DryWetProcessor)
};

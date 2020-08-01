/*
  ==============================================================================

    SingleKnob.h
    Created: 29 Jul 2020 12:17:40am
    Author:  Aidan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SingleKnob : public Component
{
public:
	SingleKnob(const float& relDiameterP) : relDiameter(relDiameterP)
	{
		addAndMakeVisible(knobSingle);
	}

	~SingleKnob() 
	{
	}

	void paint(Graphics& g) override 
	{
		//g.setColour(Colours::grey);
		//g.fillEllipse(knobArea[1]);
	}

	void resized() override 
	{
		auto width = float(getWidth());
		auto height = float(getHeight());
		auto diameter = std::min(width, height);
		auto x = (width - diameter) * .5f;
		auto y = (height - diameter) * .5f;
		knobArea[0] = juce::Rectangle<float>(x, y, diameter, diameter);
		knobSingle.setBounds(knobArea[0].toNearestInt());
		auto radius = diameter * .45f;
		auto byReducer = radius - relDiameter * radius;
		knobArea[1] = knobArea[0].reduced(byReducer + 5);
		auto thicness = .5f * (knobArea[0].getWidth() - knobArea[1].getWidth());
		auto midLine = .5f * (knobArea[1].getWidth() + thicness);
		knobSingle.getProperties().set("midLine", midLine);
		knobSingle.getProperties().set("thicness", 12.0f);
	}

	float relDiameter;

	//look::KnobSingularZeroedLAF knobSingleLAF;

	juce::Rectangle<float> knobArea[2];
	look::Slider knobSingle;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SingleKnob)
};

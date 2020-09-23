/*
  ==============================================================================

    InnerPadXY.h
    Created: 21 Aug 2020 2:02:23am
    Author:  Aidan

	actual mouse control part of pad XY because i am bad cooder and used
	awful ideas for dealing with this shit (dont do it again idiot)

  ==============================================================================
*/

#pragma once

#include "InterfaceDefines.h"

#include "GlobalVariables.h"

#include "PluginProcessor.h"

#include <JuceHeader.h>


class PadInner : public juce::Component, Timer
{
	class XYDot : public Component
	{

	protected:

		Point<float> dotXY;
		Colour colour;


		void setColour(Colour col)
		{
			colour = Colours::white;
			repaint();
		}

		void paint(Graphics& g)  override
		{
			g.fillAll(Colours::transparentBlack);
			g.setColour(colour);
			g.fillEllipse(getLocalBounds().toFloat());
		}

		friend class PadXY;
	};

public:
	PadInner();
	~PadInner() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	// mouse stuff
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseEnter(const MouseEvent& e) override;
	void mouseExit(const MouseEvent& e) override;
	void mouseDoubleClick(const MouseEvent& e) override;

	Point<int> constrainPosition(float x, float y);

	void timerCallback() override;
	
	float getValue(int axis); // gets parameter value represented by the dot's position. 0 = x, 1 = y

	void setValue(int axis, float value); // sets parameter value represented by the dot's position. 0 = x, 1 = y

	Label xLabel, yLabel;

	Point<int> mouseDownXY;
	Point<int> currentMouseXY;

	// visual dots
	XYDot dotTarget; // dragged dot on XY pad, the *target* for where the emitter is going to
	XYDot dotActual; // visual representation of the *actual* location of the sound emitter



private:

	//DopplerAudioProcessor *processor;

	int cornerSize = 10; // rounding of rectangular XY pad boundaries

	float xVal = 0, yVal = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PadInner)
};

/*
  ==============================================================================

	PadXY.h
	Created: 5 May 2020 1:55:42am
	Author:  Aidan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "InterfaceDefines.h"
#include "GlobalVariables.h"

#include "PluginProcessor.h"

#include "LookAndFeel.h"

//==============================================================================
/*
*/
class PadXY : public Component, public Slider::Listener, public Timer
{
	juce::Rectangle<float> padConstraints, dotConstraints;

	float constrainFactor = 0.075f, constrainAmount;

	float x_min, x_max, y_min, y_max, x_val, y_val;

	bool rightMouseButtonDown = false;

	// invert Y value for a traditional cartesian coordinate system (lower half is negative)
	bool yInvert = true;

	float yInverted = 0;

	bool mouseIsEntered = { false };

	Point<float> currentMouseXY;
	Point<float> mouseDownXY;

	class XYDot : public Component
	{

	protected:

		Point<float> dotXY;
		Colour colour;


		void setColour(Colour col)
		{
			colour = col;
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

	// visual dots
	XYDot dotTarget; // dragged dot on XY pad, the *target* for where the emitter is going to
	XYDot dotActual; // visual representation of the *actual* location of the sound emitter

public:

	PadXY(DopplerAudioProcessor& p);
	~PadXY();

	void paint(Graphics& g) override;
	void resized() override;

	// mouse stuff
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseEnter(const MouseEvent& e) override;
	void mouseExit(const MouseEvent& e) override;

	// target dot positioning
	Point<int> constrainPosition(float x, float y);
	Point<int> getValueAsPosition(float x, float y);

	// value setting
	void setPositionAsValue(Point<float> position);

	//bool isMouseEntered = { false };
	bool mouseDragging = { false };

	// interpolation
	void timerCallback() override;

private:

	// virtual function for slider listener
	void sliderValueChanged(Slider* slider) override;

	Slider xSlider, ySlider;
	Label xLabel, yLabel;

	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xSliderValue; // scoped pointer is depreciated in JUCE so we use this instead
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> ySliderValue;

	// smoothed values for X and Y axis of *actual location*
	// SmoothedValue<float> xSmoothed, ySmoothed;

	int cornerSize = 10; // rounding of rectangular XY pad boundaries

	look::sliderXYLAF SliderXYLAF;

	DopplerAudioProcessor& processor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PadXY)
};
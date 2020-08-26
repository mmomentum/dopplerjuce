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

#include "InnerPadXY.h"

#include "LookAndFeel.h"

//==============================================================================
/*
*/
class PadXY : public Component, public Slider::Listener, public Timer
{
	juce::Rectangle<float> padConstraints, dotConstraints;

	float constrainFactor = 0.075f, constrainAmount;

	float x_min, x_max, y_min, y_max, x_val, y_val;

public:

	PadXY(DopplerAudioProcessor& p);
	~PadXY();

	void paint(Graphics& g) override;
	void resized() override;

private:

	// virtual function for slider listener
	void sliderValueChanged(Slider* slider) override;
	void timerCallback() override;

	Slider xSlider, ySlider;
	Label xLabel, yLabel;

	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xSliderValue; // scoped pointer is depreciated in JUCE so we use this instead
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> ySliderValue;

	look::sliderXYLAF SliderXYLAF;

	PadInner dotPad;

	DopplerAudioProcessor& processor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PadXY)
};
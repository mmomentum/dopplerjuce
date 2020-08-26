/*
  ==============================================================================

	PadXY.cpp
	Created: 5 May 2020 1:55:42am
	Author:  Aidan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PadXY.h"

//Point<float> soundEmitterLocationXY;

//==============================================================================

PadXY::PadXY(DopplerAudioProcessor& p) : Component(), processor(p), Slider::Listener()
{
	setSize(XY_PAD_WIDTH, XY_PAD_HEIGHT);

	// add X / Y sliders
	xSlider.setRange(-1.0f, 1.0f);
	//xSlider.setValue(0.0f);
	xSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
	xSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	xSlider.addListener(this);
	xSlider.setLookAndFeel(&SliderXYLAF);
	addAndMakeVisible(&xSlider);

	xSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, X_ID, xSlider);

	ySlider.setRange(-1.0f, 1.0f);
	//ySlider.setValue(0.0f);
	ySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	ySlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	ySlider.addListener(this);
	ySlider.setLookAndFeel(&SliderXYLAF);
	addAndMakeVisible(&ySlider);

	ySliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, Y_ID, ySlider);

	addAndMakeVisible(dotPad);

	setInterceptsMouseClicks(false, true);

	startTimerHz(30);
}

void PadXY::sliderValueChanged(Slider* slider)
{
	if (slider == &xSlider)
	{
		if (processor.getNumParameters() > 0) // checker for parameter vector
		{
			processor.getParameters()[0]->setValue(slider->getValue()); // push to parameters vector
			dotPad.setValue(0, slider->getValue());
		}
	}
	if (slider == &ySlider)
	{
		if (processor.getNumParameters() > 1)
		{
			processor.getParameters()[1]->setValue(slider->getValue()); // push to parameters vector
			dotPad.setValue(1, slider->getValue());
		}
	}
}

void PadXY::timerCallback()
{
	//processor.getParameters()[0]->setValue(dotPad.getValue(0)); // push to parameters vector

	processor.setParameter(0, dotPad.getValue(0));

	processor.getParameters()[0]->setValue(dotPad.getValue(0)); // push to parameters vector
	xSlider.setValue(dotPad.getValue(0));

	processor.getParameters()[1]->setValue(dotPad.getValue(1)); // push to parameters vector
	ySlider.setValue(dotPad.getValue(1));
}

void PadXY::paint(Graphics& g)
{
	// XY background
	padConstraints.setPosition(0, 0);
	padConstraints.setSize(getWidth(), getHeight());

	g.setGradientFill(juce::ColourGradient(screenColorGradient, 0.0f, 0.0f,
		screenColor, 256.0f, 184.0f, false));

	g.fillRoundedRectangle(padConstraints, 10.0f);

	// fine details

	g.setGradientFill(juce::ColourGradient(Colours::transparentWhite, getWidth() / 2, getHeight() / 2,
		screenColor.darker(1.0f).withAlpha(0.5f), -getWidth() / 8, -getHeight() / 8, true));

	g.fillRoundedRectangle(padConstraints, 10.0f);

	g.setColour(whiteColor);

	g.drawRoundedRectangle(dotConstraints.reduced(constrainAmount), 10.0f, 1.5f);
}

void PadXY::resized()
{
	constrainAmount = getWidth() * constrainFactor;

	// slider bounds
	xSlider.setBounds(constrainAmount, getHeight() - constrainAmount, getWidth() - (constrainAmount * 2.0f), constrainAmount);
	ySlider.setBounds(0, constrainAmount, constrainAmount, getHeight() - (constrainAmount * 2.0f));

	dotPad.setBounds(constrainAmount, constrainAmount, getWidth() - (constrainAmount * 2), getHeight() - (constrainAmount * 2));
}

//==============================================================================

// destructor

PadXY::~PadXY()
{
	stopTimer();

	xSlider.setLookAndFeel(nullptr);
	ySlider.setLookAndFeel(nullptr);
}
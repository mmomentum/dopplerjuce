/*
  ==============================================================================

    InnerPadXY.cpp
    Created: 21 Aug 2020 2:02:23am
    Author:  Aidan

  ==============================================================================
*/

#include "InnerPadXY.h"

PadInner::PadInner() : juce::Component() , Timer()
{
	setValue(0, 0);
	setValue(1, 0);

	// add slider labels
	xLabel.setText(String(0.0, 3), dontSendNotification);
	xLabel.setJustificationType(Justification::centred);
	xLabel.setInterceptsMouseClicks(false, false);
	addAndMakeVisible(xLabel);

	yLabel.setText(String(0.0, 3), dontSendNotification);
	yLabel.setJustificationType(Justification::centred);
	yLabel.setInterceptsMouseClicks(false, false);
	addAndMakeVisible(yLabel);

	addAndMakeVisible(dotTarget);

	// set dotActual location to dotTarget's location on startup
	addAndMakeVisible(dotActual);

	repaint();

	// init graphical elements

	xLabel.setText(String("0.000", 5), dontSendNotification);
	yLabel.setText(String("0.000", 5), dontSendNotification);

	xLabel.setFont(Font("Roboto Mono", 16.00f, Font::plain));
	yLabel.setFont(Font("Roboto Mono", 16.00f, Font::plain));

	// start timer

	Timer::startTimerHz(30); // this timer's interval is up to how accurate you want the FPS to be on the interpolated dot (currently 60 fps)
}

//==============================================================================

// mouse functions (not working right as of now)

void PadInner::mouseDown(const MouseEvent& e)
{
	dotTarget.setCentrePosition(constrainPosition(e.getPosition().getX(), e.getPosition().getY()));
	mouseDownXY.setXY(dotTarget.getPosition().getX() + dotTarget.getWidth() * .5f, dotTarget.getPosition().getY() + dotTarget.getHeight() * .5f);

	//currentMouseXY = dotTarget.getPosition();

	xVal = jmap(float(mouseDownXY.getX()), dotTarget.getWidth() * .5f, getWidth() - dotTarget.getWidth() * .5f, -1.0f, 1.0f);

	yVal = jmap(float(mouseDownXY.getY()), dotTarget.getWidth() * .5f, getHeight() - dotTarget.getWidth() * .5f, 1.0f, -1.0f);
}

void PadInner::mouseDrag(const MouseEvent& e)
{
	if (e.mouseWasDraggedSinceMouseDown())
	{
		dotTarget.setCentrePosition(constrainPosition(mouseDownXY.getX() + e.getDistanceFromDragStartX(), mouseDownXY.getY() + e.getDistanceFromDragStartY()));

		xVal = jlimit(-1.0f, 1.0f, jmap(float(mouseDownXY.getX() + e.getDistanceFromDragStartX()), dotTarget.getWidth() * .5f, getWidth() - dotTarget.getWidth() * .5f, -1.0f, 1.0f));

		yVal = jlimit(-1.0f, 1.0f, jmap(float(mouseDownXY.getY() + e.getDistanceFromDragStartY()), dotTarget.getWidth() * .5f, getWidth() - dotTarget.getWidth() * .5f, 1.0f, -1.0f));
	}
}

void PadInner::mouseDoubleClick(const MouseEvent& e)
{
	yVal = 0.0f;
	xVal = 0.0f;

	dotTarget.setCentreRelative(0.5f, 0.5f);
}

void PadInner::mouseEnter(const MouseEvent& e)
{

}

void PadInner::mouseExit(const MouseEvent& e)
{

}

//==============================================================================

Point<int>  PadInner::constrainPosition(float x, float y)
{
	float xPos = jlimit(dotTarget.getWidth() / 2.0f, getWidth() - (dotTarget.getWidth() / 2.0f), x);
	float yPos = jlimit(dotTarget.getWidth() / 2.0f, getWidth() - (dotTarget.getWidth() / 2.0f), y);

	return Point<int>(xPos, yPos);
}

float PadInner::getValue(int axis)
{
	if (axis == 0)
		return xVal;
	if (axis == 1)
		return yVal;
}

void PadInner::setValue(int axis, float value)
{
	if (axis == 0)
		xVal = value;
	if (axis == 1)
		yVal = value;
}

PadInner::~PadInner()
{
	Timer::stopTimer();
}

void PadInner::paint(juce::Graphics& g)
{
	// XY background

	g.setGradientFill(juce::ColourGradient(screenColorGradient, 0.0f, 0.0f,
		screenColor, 256.0f, 184.0f, false));

	g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), cornerSize);

	// fine details

	g.setGradientFill(juce::ColourGradient(Colours::transparentWhite, getWidth() / 2, getHeight() / 2,
		screenColor.darker(1.0f).withAlpha(0.5f), -getWidth() / 8, -getHeight() / 8, true));

	g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), cornerSize);

	int rings = 12;

	for (int i = 0; i <= rings; i++)
	{
		float step = (1.0f / rings);
		float width = getWidth() * 1.5;
		float currentWidth = (width * (step * i));
		float alpha = abs((step * i) - 1);

		g.setColour(whiteColor.withAlpha(alpha));
		g.drawEllipse((getWidth() - currentWidth) / 2, (getHeight() - currentWidth) / 2, currentWidth, currentWidth, 1.5f);

		//g.setColour(whiteColor.withAlpha(alpha / 30.0f));
		//g.fillEllipse((getWidth() - currentWidth) / 2, (getHeight() - currentWidth) / 2, currentWidth, currentWidth); // how fix this?????
	}

	g.setColour(whiteColor);

	g.drawRoundedRectangle(1, 1, getWidth() - 2, getHeight() - 2, cornerSize, 2);

	g.setColour(whiteColor.withAlpha(0.5f));

	g.fillEllipse(dotTarget.getX(), dotTarget.getY(), dotTarget.getWidth(), dotTarget.getWidth());

	g.setColour(whiteColor);

	g.fillEllipse(dotActual.getX(), dotActual.getY(), dotTarget.getWidth(), dotTarget.getWidth());
}

void PadInner::resized()
{
	int test = 0;
	// labels
	xLabel.setBounds(0, getHeight(), getWidth() / 2, 20);
	yLabel.setBounds(getWidth() * .5, getHeight(), getWidth() / 2, 20);

	// dot Target
	dotTarget.setSize(cornerSize * 2, cornerSize * 2);
	dotTarget.setCentreRelative(0.5f, 0.5f);
	dotTarget.setInterceptsMouseClicks(false, false);
	//dotTarget.setAlpha(0.5f);

	// dot Actual
	dotActual.setSize(cornerSize * 2, cornerSize * 2);
	dotActual.setInterceptsMouseClicks(false, false);
	//dotActual.setColour(whiteColor);
	dotActual.setCentreRelative(0.5f, 0.5f);
}

void PadInner::timerCallback()
{
	dotTarget.setCentrePosition(	jmap(xVal, -1.0f, 1.0f, dotTarget.getWidth() / 2.0f, getWidth() - (dotTarget.getWidth() / 2.0f)),
									jmap(yVal, 1.0f, -1.0f, dotTarget.getWidth() / 2.0f, getWidth() - (dotTarget.getWidth() / 2.0f)));

	dotActual.setCentrePosition(	jmap(soundEmitterLocationXY.getX(), -1.0f, 1.0f, dotTarget.getWidth() / 2.0f, getWidth() - (dotTarget.getWidth() / 2.0f)),
									jmap(soundEmitterLocationXY.getY(), 1.0f, -1.0f, dotTarget.getWidth() / 2.0f, getWidth() - (dotTarget.getWidth() / 2.0f)));

	// set labels
	xLabel.setText(String(soundEmitterLocationXY.getX(), 3), dontSendNotification);
	yLabel.setText(String(soundEmitterLocationXY.getY(), 3), dontSendNotification);

	repaint();
}
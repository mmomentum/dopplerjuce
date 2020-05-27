/*
  ==============================================================================

	PadXY.cpp
	Created: 5 May 2020 1:55:42am
	Author:  Aidan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PadXY.h"

Point<float> soundEmitterLocationXY;

//==============================================================================

PadXY::PadXY(DopplerAudioProcessor& p)
	: Component(), processor(p), Slider::Listener(), Timer()
{
	// add X / Y sliders
	xSlider.setRange(-1.0f, 1.0f);
	xSlider.setValue(0.0f);
	xSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
	xSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	xSlider.addListener(this);
	addAndMakeVisible(&xSlider);

	xSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, X_ID, xSlider);

	ySlider.setRange(-1.0f, 1.0f);
	ySlider.setValue(0.0f);
	ySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	ySlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	ySlider.addListener(this);
	addAndMakeVisible(&ySlider);

	ySliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, Y_ID, ySlider);

	// add slider labels
	xLabel.setText(String(x_val, 3), dontSendNotification);
	xLabel.setBounds(0, padConstraints.getHeight(), getWidth() / 2, 20);
	xLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(xLabel);

	yLabel.setText(String(y_val, 3), dontSendNotification);
	yLabel.setBounds(getWidth() * .5, padConstraints.getHeight(), getWidth() / 2, 20);
	yLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(yLabel);

	setSize(XY_PAD_WIDTH, XY_PAD_HEIGHT);

	addAndMakeVisible(dotTarget);
	setPositionAsValue(dotTarget.getPosition().toFloat());

	// set dotActual location to dotTarget's location on startup
	addAndMakeVisible(dotActual);
	setPositionAsValue(dotTarget.getPosition().toFloat());
	repaint();

	// init graphical elements

	xSlider.setValue(0);
	ySlider.setValue(0);

	xLabel.setText(String("0.000", 5), dontSendNotification);
	yLabel.setText(String("0.000", 5), dontSendNotification);

	// start timer

	Timer::startTimerHz(60);
}

//==============================================================================

// mouse functions

void PadXY::mouseDown(const MouseEvent& e)
{
	dotTarget.setTopLeftPosition(constrainPosition(e.getPosition().getX(), e.getPosition().getY()));
	mouseDownXY.setXY(dotTarget.getPosition().getX() + dotTarget.getWidth() * .5f, dotTarget.getPosition().getY() + dotTarget.getHeight() * .5f);
	setPositionAsValue(dotTarget.getPosition().toFloat());
	repaint();

	currentMouseXY = dotTarget.getPosition().toFloat();

	xSlider.setValue(jmap(currentMouseXY.getX(), 0.0f, padConstraints.getWidth() - (BORDER_SIZE)-0.0f, -1.0f, 1.0f));
	ySlider.setValue(jmap(currentMouseXY.getY(), 0.0f, padConstraints.getHeight() - (BORDER_SIZE)-0.0f, 1.0f, -1.0f));
}

void PadXY::mouseDrag(const MouseEvent& e)
{
	if (e.mouseWasDraggedSinceMouseDown())
	{
		mouseDragging = true;

		dotTarget.setTopLeftPosition(constrainPosition(mouseDownXY.getX() + e.getDistanceFromDragStartX(), mouseDownXY.getY() + e.getDistanceFromDragStartY()));
		setPositionAsValue(dotTarget.getPosition().toFloat());
		repaint();

		// set current mouse coordinates
		currentMouseXY = dotTarget.getPosition().toFloat();

		// put in a function for doing interpolation of the X and Y values and converting them to a parameter.
		//xSmoothed.setTargetValue(currentMouseXY.getX());
		//ySmoothed.setTargetValue(currentMouseXY.getY());

		// figure out how to use getvalues for the width as it doesnt accept them currently
		xSlider.setValue(jmap(currentMouseXY.getX(), 0.0f, padConstraints.getWidth() - (BORDER_SIZE)-0.0f, -1.0f, 1.0f));
		ySlider.setValue(jmap(currentMouseXY.getY(), 0.0f, padConstraints.getHeight() - (BORDER_SIZE)-0.0f, 1.0f, -1.0f));
	}

	mouseDragging = false;
}

void PadXY::mouseEnter(const MouseEvent& e)
{
	mouseIsEntered = true;
}

void PadXY::mouseExit(const MouseEvent& e)
{
	mouseIsEntered = false;
}

//==============================================================================

Point<int> PadXY::constrainPosition(float x, float y)
{
	const float xPos = jlimit(padConstraints.getX(), (padConstraints.getWidth() + padConstraints.getX()) - dotTarget.getWidth(), x - dotTarget.getWidth() / 2.f);
	const float yPos = jlimit(padConstraints.getY(), (padConstraints.getHeight() + padConstraints.getY()) - dotTarget.getHeight(), y - dotTarget.getHeight() / 2.f);

	return Point<int>(xPos, yPos);
}

Point<int> PadXY::getValueAsPosition(float x, float y)
{
	const float xPos = jmap(x, padConstraints.getX(), (padConstraints.getWidth() + padConstraints.getX()) - dotTarget.getWidth());
	const float yPos = jmap(y, padConstraints.getY(), (padConstraints.getHeight() + padConstraints.getY()) - dotTarget.getHeight());

	if (yInvert) y = y_max - y;

	return Point<int>(xPos, yPos);
}

void PadXY::setPositionAsValue(Point<float> position)
{
	const float xVal = jlimit(x_min, x_max, jmap(position.getX(), padConstraints.getX(), padConstraints.getWidth() - dotTarget.getWidth(), x_min, x_max));
	float       yVal = jlimit(y_min, y_max, jmap(position.getY(), padConstraints.getY(), padConstraints.getHeight() - dotTarget.getHeight(), y_min, y_max));

	if (yInvert) yVal = y_max - yVal;
}

void PadXY::sliderValueChanged(Slider* slider)
{
	if (slider == &xSlider)
	{
		if (processor.getNumParameters() > 0) // checker for parameter vector
			processor.getParameters()[0]->setValue(slider->getValue()); // push to parameters vector
		//xLabel.setText(String(slider->getValue(), 3), dontSendNotification); // set x label
	}
	if (slider == &ySlider)
	{
		if (processor.getNumParameters() > 1)
			processor.getParameters()[1]->setValue(slider->getValue()); // push to parameters vector
		//yLabel.setText(String(slider->getValue(), 3), dontSendNotification); // set y label
	}

	// set slider control values to the dot's graphical position
	if (mouseDragging == false)
	{
		dotTarget.setCentrePosition(jmap((float)xSlider.getValue(), -1.0f, 1.0f, 10.0f, (XY_PAD_WIDTH - 50.0f)), 
									jmap((float)ySlider.getValue() * -1.0f, -1.0f, 1.0f, 10.0f, (XY_PAD_HEIGHT - 50.0f)));
	}
}

void PadXY::timerCallback()
{
	// internal GUI based parameter use (smoothing)
	auto smoothingValue = processor.treeState.getRawParameterValue(SMOOTH_ID); 

	Point<int> diff = dotTarget.getPosition() - dotActual.getPosition();
	float timePortion = getTimerInterval() / (smoothingValue[0] / 2);

	interpolationMovementAmount.setXY(diff.getX() * timePortion, diff.getY() * timePortion);
	dotActual.dotXY += interpolationMovementAmount;

	// set dot location
	dotActual.setCentrePosition(dotActual.dotXY.getX(),dotActual.dotXY.getY());

	// repaint
	dotActual.repaint();

	// set actual sound emitter location to the almost the same standard as the original 
	// slider values, almost an exact reverse mapping from the sliderValueChanged function.
	soundEmitterLocationXY.setXY(jmap(dotActual.getX() - 0.0f, 10.0f, (XY_PAD_WIDTH  - 50.0f), -1.0f, 1.0f) + 0.059, 
								(jmap(dotActual.getY() - 0.0f, 10.0f, (XY_PAD_HEIGHT - 50.0f), -1.0f, 1.0f) + 0.059) * -1.0f);

	// for some reason the center is off by a tiny bit in a lot of the code. the + 0.059 fixes it but its a shitty kludge

	// set labels for testing
	xLabel.setText(String(soundEmitterLocationXY.getX(), 3), dontSendNotification);
	yLabel.setText(String(soundEmitterLocationXY.getY(), 3), dontSendNotification);
}

void PadXY::paint(Graphics& g)
{
	// Text labels
	g.setColour(Colours::white);

	// XY background
	padConstraints.setPosition(0, 0);
	padConstraints.setSize(getWidth(), getHeight());

	g.setColour(Colours::black);
	g.fillRoundedRectangle(padConstraints, cornerSize);

	g.setColour(Colours::white);
	g.drawEllipse((padConstraints.getWidth() - 10)/ 2, (padConstraints.getHeight() - 10)/ 2, 10, 10, 3);
}

void PadXY::resized()
{
	//background padConstraints that labels use for reference
	padConstraints.setWidth(getWidth() * 8);
	padConstraints.setHeight(getHeight() * .85);
	padConstraints.setTop(getHeight() * .02);
	padConstraints.setLeft(getWidth() * .02);

	// dot Target
	dotTarget.setSize(20, 20);
	const Point<int> pos(getValueAsPosition(x_val, y_val));
	dotTarget.setCentreRelative(0.5f, 0.5f);
	dotTarget.setInterceptsMouseClicks(false, false);
	dotTarget.setColour(Colours::grey);
	dotTarget.setAlpha(0.8);

	// dot Actual
	dotActual.setSize(20, 20);
	dotActual.setInterceptsMouseClicks(false, false);
	dotActual.setColour(Colours::white);
	dotActual.setAlpha(0.8);
	dotActual.setCentreRelative(0.5f, 0.5f);

	repaint();

	// slider bounds
	xSlider.setBounds(BORDER_SIZE / 2, BORDER_SIZE / 4, getWidth() - BORDER_SIZE, BORDER_SIZE);
	ySlider.setBounds(BORDER_SIZE / 4, BORDER_SIZE / 2, BORDER_SIZE / 4, getHeight() - BORDER_SIZE);

	// labels
	xLabel.setBounds(0, padConstraints.getHeight(), getWidth() / 2, 20);
	yLabel.setBounds(getWidth() * .5, padConstraints.getHeight(), getWidth() / 2, 20);
}

//==============================================================================

// destructor

PadXY::~PadXY()
{
	Timer::stopTimer();
}
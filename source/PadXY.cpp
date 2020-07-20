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

PadXY::PadXY(DopplerAudioProcessor& p)
	: Component(), processor(p), Slider::Listener(), Timer()
{
	setSize(XY_PAD_WIDTH, XY_PAD_HEIGHT);

	dotConstraints.setSize(getWidth(), getHeight());

	// add X / Y sliders
	xSlider.setRange(-1.0f, 1.0f);
	xSlider.setValue(0.0f);
	xSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
	xSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	xSlider.addListener(this);
	xSlider.setLookAndFeel(&SliderXYLAF);
	addAndMakeVisible(&xSlider);

	xSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, X_ID, xSlider);

	ySlider.setRange(-1.0f, 1.0f);
	ySlider.setValue(0.0f);
	ySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	ySlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	ySlider.addListener(this);
	ySlider.setLookAndFeel(&SliderXYLAF);
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

	xLabel.setFont(Font ("Roboto Mono", 16.00f, Font::plain));
	yLabel.setFont(Font ("Roboto Mono", 16.00f, Font::plain));

	// start timer

	Timer::startTimerHz(60); // this timer's interval is up to how accurate you want the FPS to be on the interpolated dot (currently 60 fps)
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

	xSlider.setValue(jmap(currentMouseXY.getX(), constrainAmount, padConstraints.getWidth()- (constrainAmount * 1.0f), -1.0f, 1.0f));
	ySlider.setValue(jmap(currentMouseXY.getY(), constrainAmount, padConstraints.getHeight() - (constrainAmount * 1.0f), 1.0f, -1.0f));
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
		xSlider.setValue(jmap(currentMouseXY.getX(), constrainAmount, padConstraints.getWidth() - (constrainAmount * 1.0f), -1.0f, 1.0f));
		ySlider.setValue(jmap(currentMouseXY.getY(), constrainAmount, padConstraints.getHeight() - (constrainAmount * 1.0f), 1.0f, -1.0f));
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
	const float xPos = jlimit(dotConstraints.getX(), (dotConstraints.getWidth() + dotConstraints.getX()) - dotTarget.getWidth(), x - dotTarget.getWidth() / 2.f);
	const float yPos = jlimit(dotConstraints.getY(), (dotConstraints.getHeight() + dotConstraints.getY()) - dotTarget.getHeight(), y - dotTarget.getHeight() / 2.f);

	return Point<int>(xPos, yPos);
}

Point<int> PadXY::getValueAsPosition(float x, float y)
{
	const float xPos = jmap(x, dotConstraints.getX(), (dotConstraints.getWidth() + dotConstraints.getX()) - dotTarget.getWidth());
	const float yPos = jmap(y, dotConstraints.getY(), (dotConstraints.getHeight() + dotConstraints.getY()) - dotTarget.getHeight());

	if (yInvert) y = y_max - y;

	return Point<int>(xPos, yPos);
}

void PadXY::setPositionAsValue(Point<float> position)
{
	const float xVal = jlimit(x_min, x_max, jmap(position.getX(), dotConstraints.getX(), dotConstraints.getWidth() - dotTarget.getWidth(), x_min, x_max));
	float       yVal = jlimit(y_min, y_max, jmap(position.getY(), dotConstraints.getY(), dotConstraints.getHeight() - dotTarget.getHeight(), y_min, y_max));

	if (yInvert) yVal = y_max - yVal;
}

void PadXY::sliderValueChanged(Slider* slider)
{
	if (slider == &xSlider)
	{
		if (processor.getNumParameters() > 0) // checker for parameter vector
			processor.getParameters()[0]->setValue(slider->getValue()); // push to parameters vector
	}
	if (slider == &ySlider)
	{
		if (processor.getNumParameters() > 1)
			processor.getParameters()[1]->setValue(slider->getValue()); // push to parameters vector
	}

	// set slider control values to the dot's graphical position
	if (mouseDragging == false)
	{
		dotTarget.setCentrePosition(jmap((float)xSlider.getValue(), -1.0f, 1.0f, constrainAmount + cornerSize, XY_PAD_WIDTH - (constrainAmount * 1.0f) - cornerSize),
									jmap((float)ySlider.getValue() * -1.0f, -1.0f, 1.0f, constrainAmount + cornerSize, XY_PAD_HEIGHT - (constrainAmount * 1.0f) - cornerSize));
	}
}

void PadXY::timerCallback()
{
	// set dot location
	dotActual.setCentrePosition(jmap(soundEmitterLocationXY.getX(), -1.0f, 1.0f, constrainAmount + cornerSize, XY_PAD_WIDTH - (constrainAmount * 1.0f) - cornerSize),
								jmap(soundEmitterLocationXY.getY() * -1.0f, -1.0f, 1.0f, constrainAmount + cornerSize, XY_PAD_HEIGHT - (constrainAmount * 1.0f) - cornerSize));

	// seeing a bit of inaccuracy after the move to the audioprocessor but its probably just a jmap target range issue

	// repaint
	dotActual.repaint();

	// set labels
	xLabel.setText(String(soundEmitterLocationXY.getX(), 3), dontSendNotification);
	yLabel.setText(String(soundEmitterLocationXY.getY(), 3), dontSendNotification);
}

void PadXY::paint(Graphics& g)
{

	// XY background
	padConstraints.setPosition(0, 0);
	padConstraints.setSize(getWidth(), getHeight());

	g.setGradientFill(juce::ColourGradient(screenColorGradient, 0.0f, 0.0f,
		screenColor, 256.0f, 184.0f, false));

	g.fillRoundedRectangle(padConstraints, cornerSize);

	// fine details

	

	int rings = 12;

	for (int i = 0; i <= rings; i++)
	{
		float step = (1.0f / rings);
		float width = dotConstraints.getWidth() - constrainAmount;
		float currentWidth = (width * (step * i));
		float alpha = abs((step * i) - 1);

		g.setColour(whiteColor.withAlpha(alpha));
		g.drawEllipse((padConstraints.getWidth() - currentWidth) / 2, (padConstraints.getHeight() - currentWidth) / 2, currentWidth, currentWidth, 1.5f);
	}

	g.setColour(whiteColor);

	g.drawRoundedRectangle(dotConstraints.reduced(constrainAmount), cornerSize, 1.5f);

	g.setGradientFill(juce::ColourGradient(whiteColor.withAlpha(0.5f), getWidth() / 2, getHeight() / 2,
		Colours::transparentWhite, constrainAmount * 1.5f, getWidth() / 2, true));

	// would do this with normal lines but you cant do gradient fills with them

	Path line;

	// up / down
	//line.startNewSubPath(dotConstraints.getWidth() / 2, constrainAmount);
	//line.lineTo(dotConstraints.getWidth() / 2, dotConstraints.getHeight() - constrainAmount);

	// left / right
	//line.startNewSubPath(constrainAmount, dotConstraints.getHeight() / 2);
	//line.lineTo(dotConstraints.getWidth() - constrainAmount, dotConstraints.getHeight() / 2);

	//g.strokePath(line, { 1.5f, PathStrokeType::curved, PathStrokeType::rounded });
}

void PadXY::resized()
{
	//background padConstraints that labels use for reference
	padConstraints.setWidth(getWidth() * 8);
	padConstraints.setHeight(getHeight() * .85);
	padConstraints.setTop(getHeight() * .02);
	padConstraints.setLeft(getWidth() * .02);

	// dot Target
	dotTarget.setSize(cornerSize * 2, cornerSize * 2);
	const Point<int> pos(getValueAsPosition(x_val, y_val));
	dotTarget.setCentreRelative(0.5f, 0.5f);
	dotTarget.setInterceptsMouseClicks(false, false);
	dotTarget.setColour(whiteColor.withAlpha(0.5f));

	// dot Actual
	dotActual.setSize(cornerSize * 2, cornerSize * 2);
	dotActual.setInterceptsMouseClicks(false, false);
	dotActual.setColour(whiteColor);
	dotActual.setCentreRelative(0.5f, 0.5f);

	repaint();

	constrainAmount = getWidth() * constrainFactor;

	// slider bounds
	xSlider.setBounds(constrainAmount, 0, getWidth() - (constrainAmount * 2.0f), constrainAmount);
	ySlider.setBounds(0, constrainAmount, constrainAmount, getHeight() - (constrainAmount * 2.0f));

	// labels
	xLabel.setBounds(0, padConstraints.getHeight(), getWidth() / 2, 20);
	yLabel.setBounds(getWidth() * .5, padConstraints.getHeight(), getWidth() / 2, 20);
}

//==============================================================================

// destructor

PadXY::~PadXY()
{
	Timer::stopTimer();
	xSlider.setLookAndFeel(nullptr);
	ySlider.setLookAndFeel(nullptr);
}
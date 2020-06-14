/*
  ==============================================================================

    ParameterPanel.cpp
    Created: 8 May 2020 9:23:17pm
    Author:  Aidan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ParameterPanel.h"

//==============================================================================

ParameterPanel::ParameterPanel(DopplerAudioProcessor& p)
	: Component(), processor(p)
{
	// add knobs

	// knob representing the XY pad's "size" (in meters)
	sizeSlider.setRange(5.0f, 100.0f);
	sizeSlider.setValue(20.0f);
	sizeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	sizeSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 60, 20);
	sizeSlider.addListener(this);
	addAndMakeVisible(&sizeSlider);

	sizeSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, SIZE_ID, sizeSlider);

	// knob for smoothing of actual position (in ms)
	smoothSlider.setRange(100, 5000);
	smoothSlider.setValue(500);
	smoothSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	smoothSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 60, 20);
	smoothSlider.addListener(this);
	addAndMakeVisible(&smoothSlider);

	smoothSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, SMOOTH_ID, smoothSlider);

	// knob for setting distance between listening points (in cm)
	distanceSlider.setRange(5.0f, 100.0f);
	distanceSlider.setValue(9.0f);
	distanceSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	distanceSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 60, 20);
	distanceSlider.addListener(this);
	addAndMakeVisible(&distanceSlider);

	distanceSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, DISTANCE_ID, distanceSlider);

}

void ParameterPanel::sliderValueChanged(Slider* slider)
{
	if (slider == &sizeSlider)
	{
		if (processor.getNumParameters() > 2) // parameter size checker
			processor.getParameters()[2]->setValue(slider->getValue()); // push to parameters vector
	}
	if (slider == &smoothSlider)
	{
		if (processor.getNumParameters() > 3)
			processor.getParameters()[3]->setValue(slider->getValue());
	}
	if (slider == &distanceSlider)
	{
		if (processor.getNumParameters() > 4)
			processor.getParameters()[4]->setValue(slider->getValue());
	}
}

void ParameterPanel::paint (Graphics& g)
{

}

void ParameterPanel::resized()
{
	sizeSlider.setBounds(0 - BORDER_SIZE, 0, PARAMETER_PANEL_WIDTH / 3, PARAMETER_PANEL_HEIGHT - BORDER_SIZE);

	smoothSlider.setBounds((PARAMETER_PANEL_WIDTH / 3) - BORDER_SIZE, 0, PARAMETER_PANEL_WIDTH / 3, PARAMETER_PANEL_HEIGHT - BORDER_SIZE);

	distanceSlider.setBounds(((PARAMETER_PANEL_WIDTH / 3) * 2) - BORDER_SIZE, 0, PARAMETER_PANEL_WIDTH / 3, PARAMETER_PANEL_HEIGHT - BORDER_SIZE);
}

ParameterPanel::~ParameterPanel()
{

}
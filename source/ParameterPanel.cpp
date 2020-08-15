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
	{
		addAndMakeVisible(sliderLabel[0]);
		addAndMakeVisible(sliderName[0]);
		addAndMakeVisible(&sizeSlider);

		// name label

		sliderName[0].setJustificationType(juce::Justification::centred);
		sliderName[0].setFont(Font("MuseoModerno", labelSize * textMultiplier, Font::plain));
		sliderName[0].setColour(Label::textColourId, greyColor);
		sliderName[0].setText("Size", dontSendNotification);

		// value label

		sliderLabel[0].setJustificationType(juce::Justification::centred);
		sliderLabel[0].setFont(Font("Roboto Mono", labelSize * 1.2, Font::plain));
		sliderLabel[0].setColour(Label::textColourId, whiteColor);

		// slider

		sizeSlider.setRange(5.0f, 100.0f);
		sizeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
		sizeSlider.setTextBoxStyle(Slider::NoTextBox, true, 60, 20);
		sizeSlider.addListener(this);
		sizeSlider.setLookAndFeel(&sliderLAF);

		// attachment

		sizeSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, SIZE_ID, sizeSlider);
	}
	// knob for smoothing of actual position (in ms)
	{
		addAndMakeVisible(sliderLabel[1]);
		addAndMakeVisible(sliderName[1]);
		addAndMakeVisible(smoothSlider);

		sliderName[1].setJustificationType(juce::Justification::centred);
		sliderName[1].setFont(Font("MuseoModerno", labelSize * textMultiplier, Font::plain));
		sliderName[1].setColour(Label::textColourId, greyColor);
		sliderName[1].setText("Smoothing", dontSendNotification);

		sliderLabel[1].setJustificationType(juce::Justification::centred);
		sliderLabel[1].setFont(Font("Roboto Mono", labelSize * 1.2, Font::plain));
		sliderLabel[1].setColour(Label::textColourId, whiteColor);


		smoothSlider.setRange(100, 5000);
		smoothSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
		smoothSlider.setTextBoxStyle(Slider::NoTextBox, true, 60, 20);
		smoothSlider.addListener(this);
		smoothSlider.setLookAndFeel(&sliderLAF);

		// attachment

		smoothSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, SMOOTH_ID, smoothSlider);
	}
	// knob for minimum filtering amount (turn to max to stop filtering, basically)
	{
		addAndMakeVisible(sliderLabel[2]);
		addAndMakeVisible(sliderName[2]);
		addAndMakeVisible(minFreqSlider);

		sliderName[2].setJustificationType(juce::Justification::centred);
		sliderName[2].setFont(Font("MuseoModerno", labelSize * textMultiplier, Font::plain));
		sliderName[2].setColour(Label::textColourId, greyColor);
		sliderName[2].setText("Min Hz", dontSendNotification);

		sliderLabel[2].setJustificationType(juce::Justification::centred);
		sliderLabel[2].setFont(Font("Roboto Mono", labelSize * 1.2, Font::plain));
		sliderLabel[2].setColour(Label::textColourId, whiteColor);

		minFreqSlider.setRange(1500.0f, 22000.0f);
		minFreqSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
		minFreqSlider.setTextBoxStyle(Slider::NoTextBox, true, 60, 20);
		minFreqSlider.addListener(this);
		minFreqSlider.setLookAndFeel(&sliderLAF);

		minFreqSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, FILTER_ID, minFreqSlider);
	}
	// knob for doppler factor (multiplies delay time by knob amount (where 1 = normal factor))
	{
		addAndMakeVisible(sliderLabel[3]);
		addAndMakeVisible(sliderName[3]);
		addAndMakeVisible(dopplerFactorSlider);

		sliderName[3].setJustificationType(juce::Justification::centred);
		sliderName[3].setFont(Font("MuseoModerno", labelSize * textMultiplier, Font::plain));
		sliderName[3].setColour(Label::textColourId, greyColor);
		sliderName[3].setText("Factor", dontSendNotification);

		sliderLabel[3].setJustificationType(juce::Justification::centred);
		sliderLabel[3].setFont(Font("Roboto Mono", labelSize * 1.2, Font::plain));
		sliderLabel[3].setColour(Label::textColourId, whiteColor);

		dopplerFactorSlider.setRange(0.0f, 3.0f);
		dopplerFactorSlider.setValue(1.0f);
		dopplerFactorSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
		dopplerFactorSlider.setTextBoxStyle(Slider::NoTextBox, true, 60, 20);
		dopplerFactorSlider.addListener(this);
		dopplerFactorSlider.setLookAndFeel(&sliderLAF);

		dopplerFactorSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, FACTOR_ID, dopplerFactorSlider);
	}
	{
		addAndMakeVisible(sliderLabel[4]);
		addAndMakeVisible(sliderName[4]);
		addAndMakeVisible(dryWetSlider);

		sliderName[4].setJustificationType(juce::Justification::centred);
		sliderName[4].setFont(Font("MuseoModerno", labelSize * textMultiplier, Font::plain));
		sliderName[4].setColour(Label::textColourId, greyColor);
		sliderName[4].setText("Mix", dontSendNotification);

		sliderLabel[4].setJustificationType(juce::Justification::centred);
		sliderLabel[4].setFont(Font("Roboto Mono", labelSize * 1.2, Font::plain));
		sliderLabel[4].setColour(Label::textColourId, whiteColor);
		


		dryWetSlider.setRange(0.0f, 100.0f);
		dryWetSlider.setValue(100.0f);
		dryWetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
		dryWetSlider.setTextBoxStyle(Slider::NoTextBox, true, 60, 20);
		dryWetSlider.addListener(this);
		dryWetSlider.setLookAndFeel(&sliderLAF);

		dryWetSliderValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.treeState, DRYWET_ID, dryWetSlider);
	}


	// add buttons

	addAndMakeVisible(pitchingModeButton);
	pitchingModeButton.setButtonText(TRANS("Mode"));
	pitchingModeButton.addListener(this);
	pitchingModeButton.setClickingTogglesState(true);
	pitchingModeButton.setLookAndFeel(&buttonLAF);

	pitchingModeButtonValue = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(p.treeState, MODE_ID, pitchingModeButton);

	addAndMakeVisible(dopplerToggleButton);
	dopplerToggleButton.setButtonText(TRANS("Doppler"));
	dopplerToggleButton.addListener(this);
	dopplerToggleButton.setClickingTogglesState(true);
	dopplerToggleButton.setLookAndFeel(&buttonLAF);

	dopplerToggleButtonValue = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(p.treeState, DOPPLER_ID, dopplerToggleButton);

	addAndMakeVisible(ampToggleButton);
	ampToggleButton.setButtonText(TRANS("Volume"));
	ampToggleButton.addListener(this);
	ampToggleButton.setClickingTogglesState(true);
	ampToggleButton.setLookAndFeel(&buttonLAF);

	ampToggleButtonValue = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(p.treeState, VOLUME_ID, ampToggleButton);

	addAndMakeVisible(hrtfToggleButton);
	hrtfToggleButton.setButtonText(TRANS("Spatial"));
	hrtfToggleButton.addListener(this);
	hrtfToggleButton.setClickingTogglesState(true);
	hrtfToggleButton.setLookAndFeel(&buttonLAF);

	hrtfToggleButtonValue = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(p.treeState, HRTF_ID, hrtfToggleButton);

	setSize(PARAMETER_PANEL_WIDTH - (BORDER_SIZE * 2), PARAMETER_PANEL_WIDTH);

}

void ParameterPanel::sliderValueChanged(Slider* slider)
{
	if (slider == &sizeSlider)
	{
		if (processor.getNumParameters() > 2) // parameter size checker
		{
			processor.getParameters()[2]->setValue(slider->getValue()); // push to parameters vector
			sliderLabel[0].setText(String(slider->getValue(), 1) + "M", dontSendNotification);
		}
	}
	if (slider == &smoothSlider)
	{
		if (processor.getNumParameters() > 3)
		{
			processor.getParameters()[3]->setValue(slider->getValue());

			if (int(slider->getValue()) < 1000)
				sliderLabel[1].setText(String(slider->getValue(), 1) + "ms", dontSendNotification);
			if (int(slider->getValue()) > 1000)
				sliderLabel[1].setText(String(slider->getValue() / 1000.0f, 1) + "s", dontSendNotification);
		}
	}
	if (slider == &minFreqSlider)
	{
		if (processor.getNumParameters() > 5)
		{
			processor.getParameters()[5]->setValue(slider->getValue());

			if (int(slider->getValue()) < 10000)
				sliderLabel[2].setText(String(slider->getValue() / 1000.0f, 2) + "KHz", dontSendNotification);
			if (int(slider->getValue()) > 10000)
				sliderLabel[2].setText(String(slider->getValue() / 1000.0f, 1) + "KHz", dontSendNotification);
		}
	}
	if (slider == &dopplerFactorSlider)
	{
		if (processor.getNumParameters() > 10)
		{
			processor.getParameters()[10]->setValue(slider->getValue());
			sliderLabel[3].setText(String(slider->getValue(), 2), dontSendNotification);
		}
	}
	if (slider == &dryWetSlider)
	{
		//if (processor.getNumParameters() > 2) // parameter size checker
		//{
			//processor.getParameters()[2]->setValue(slider->getValue()); // push to parameters vector
			sliderLabel[4].setText(String(slider->getValue(), 1) + "%", dontSendNotification);
		//}
	}
}

void ParameterPanel::buttonClicked(Button* button)
{
	if (button == &pitchingModeButton)
	{
		processor.getParameters()[6]->setValue(int(pitchingModeButton.getToggleState()));
	}
	if (button == &hrtfToggleButton)
	{
		processor.getParameters()[7]->setValue(int(hrtfToggleButton.getToggleState()));
	}
	if (button == &ampToggleButton)
	{
		processor.getParameters()[8]->setValue(int(ampToggleButton.getToggleState()));
	}
	if (button == &dopplerToggleButton)
	{
		processor.getParameters()[9]->setValue(int(dopplerToggleButton.getToggleState()));
	}
}

void ParameterPanel::paint(Graphics& g)
{

}

void ParameterPanel::resized()
{
	// sliders
	{
		{
			// plane size slider
			sliderBounds[0].setBounds(0, 0, getWidth() / 5.0f, (getHeight() / 3) * 2.0f);

			sizeSlider.setBounds(sliderBounds[0]);
			sliderLabel[0].setBounds(sliderBounds[0]);

			sliderBounds[0].translate(0, sliderBounds[0].getHeight() * sliderNameOffset);
			sliderName[0].setBounds(sliderBounds[0]);
		}
		{
			// smoothing slider
			sliderBounds[1].setBounds((getWidth() / 5.0f), 0, getWidth() / 5.0f, (getHeight() / 3) * 2.0f);

			smoothSlider.setBounds(sliderBounds[1]);
			sliderLabel[1].setBounds(sliderBounds[1]);

			sliderBounds[1].translate(0, sliderBounds[1].getHeight() * sliderNameOffset);
			sliderName[1].setBounds(sliderBounds[1]);
		}
		{
			// min freq
			sliderBounds[2].setBounds(((getWidth() / 5.0f) * 2.0f), 0, getWidth() / 5.0f, (getHeight() / 3) * 2.0f);

			minFreqSlider.setBounds(sliderBounds[2]);
			sliderLabel[2].setBounds(sliderBounds[2]);

			sliderBounds[2].translate(0, sliderBounds[2].getHeight() * sliderNameOffset);
			sliderName[2].setBounds(sliderBounds[2]);
		}
		{
			// doppler factor
			sliderBounds[3].setBounds(((getWidth() / 5.0f) * 3.0f), 0, getWidth() / 5.0f, (getHeight() / 3) * 2.0f);

			dopplerFactorSlider.setBounds(sliderBounds[3]);
			sliderLabel[3].setBounds(sliderBounds[3]);

			sliderBounds[3].translate(0, sliderBounds[3].getHeight() * sliderNameOffset);
			sliderName[3].setBounds(sliderBounds[3]);
		}
		{
			// dry wet slider
			sliderBounds[4].setBounds(((getWidth() / 5.0f) * 4.0f), 0, getWidth() / 5.0f, (getHeight() / 3) * 2.0f);

			dryWetSlider.setBounds(sliderBounds[4]);
			sliderLabel[4].setBounds(sliderBounds[4]);

			sliderBounds[4].translate(0, sliderBounds[4].getHeight() * sliderNameOffset);
			sliderName[4].setBounds(sliderBounds[4]);
		}
	}
	// buttons
	{
		buttonBounds[0].setBounds(0, (getHeight() / 3) * 2.0f, (getWidth() / 4.0f), getHeight() / 3);

		buttonBounds[1].setBounds((getWidth() / 4.0f), (getHeight() / 3) * 2.0f, (getWidth() / 4.0f), getHeight() / 3);

		buttonBounds[2].setBounds((getWidth() / 4.0f) * 2, (getHeight() / 3) * 2.0f, (getWidth() / 4.0f), getHeight() / 3);

		buttonBounds[3].setBounds((getWidth() / 4.0f) * 3, (getHeight() / 3) * 2.0f, (getWidth() / 4.0f), getHeight() / 3);

		pitchingModeButton.setBounds(buttonBounds[0].reduced(10));

		dopplerToggleButton.setBounds(buttonBounds[1].reduced(10));

		ampToggleButton.setBounds(buttonBounds[2].reduced(10));

		hrtfToggleButton.setBounds(buttonBounds[3].reduced(10));
	}
}

ParameterPanel::~ParameterPanel()
{
	// remove styling to prevent issues

	sizeSlider.setLookAndFeel(nullptr);
	smoothSlider.setLookAndFeel(nullptr);
	minFreqSlider.setLookAndFeel(nullptr);
	dopplerFactorSlider.setLookAndFeel(nullptr);
	dryWetSlider.setLookAndFeel(nullptr);

	pitchingModeButton.setLookAndFeel(nullptr);
	dopplerToggleButton.setLookAndFeel(nullptr);
	ampToggleButton.setLookAndFeel(nullptr);
	hrtfToggleButton.setLookAndFeel(nullptr);
}
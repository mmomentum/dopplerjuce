/*
  ==============================================================================

    ParameterPanel.h
    Created: 8 May 2020 9:23:17pm
    Author:  Aidan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "InterfaceDefines.h"

#include "PluginProcessor.h"

#include "LookAndFeel.h"

//==============================================================================
/*
*/
class ParameterPanel : public Component, public Slider::Listener, public Button::Listener
{
public:
    ParameterPanel(DopplerAudioProcessor& p);
    ~ParameterPanel();

    void paint (Graphics&) override;
    void resized() override;
	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(Button* button) override;

	Label sliderLabel[5]; // dynamic value labels

private:

	Slider sizeSlider;
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sizeSliderValue; // scoped pointer is depreciated in JUCE so we use this instead

	Slider smoothSlider;
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> smoothSliderValue;

	Slider minFreqSlider;
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> minFreqSliderValue;

	Slider dopplerFactorSlider;
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> dopplerFactorSliderValue;

	Slider dryWetSlider;
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> dryWetSliderValue;

	TextButton pitchingModeButton;
	std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> pitchingModeButtonValue;

	TextButton hrtfToggleButton;
	std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> hrtfToggleButtonValue;

	TextButton ampToggleButton;
	std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> ampToggleButtonValue;

	TextButton dopplerToggleButton;
	std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> dopplerToggleButtonValue;

	juce::Rectangle<int> buttonBounds[4];

	juce::Rectangle<int> sliderBounds[5];

	Label sliderName[5]; // slider names

	look::ButtonLAF buttonLAF;
	look::SliderLAF sliderLAF;

	float	labelSize = 12.0f,
			textMultiplier = 1.3f,
			sliderNameOffset = 0.45f;

	int		reduceFactor = 5;

	DopplerAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterPanel)

};

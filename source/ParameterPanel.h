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

//==============================================================================
/*
*/
class ParameterPanel : public Component, public Slider::Listener
{
public:
    ParameterPanel(DopplerAudioProcessor& p);
    ~ParameterPanel();

    void paint (Graphics&) override;
    void resized() override;

private:

	Slider sizeSlider;
	Slider smoothSlider;
	Slider distanceSlider;

	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sizeSliderValue; // scoped pointer is depreciated in JUCE so we use this instead
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> smoothSliderValue;
	std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> distanceSliderValue;

	//Label sideLabel;
	//Label smoothLabel;
	//Label distanceLabel;

	void sliderValueChanged(Slider* slider);

	DopplerAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterPanel)

};

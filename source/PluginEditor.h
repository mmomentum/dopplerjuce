/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include "PadXY.h" // xy component
#include "ParameterPanel.h" // additional parameters component 

#include "InterfaceDefines.h" // interface defines

//==============================================================================

class DopplerAudioProcessorEditor : public AudioProcessorEditor
	//public Slider::Listener//,
  //public MouseEvent::Listener // we might need this but idk yet
{
public:

	DopplerAudioProcessorEditor(DopplerAudioProcessor&);
	~DopplerAudioProcessorEditor();

	void paint(Graphics& g) override;
	void resized() override;
	
	std::unique_ptr<PadXY> padxy;
	std::unique_ptr<ParameterPanel> parampanel;

private: 
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	
	DopplerAudioProcessor& processor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DopplerAudioProcessorEditor)
};

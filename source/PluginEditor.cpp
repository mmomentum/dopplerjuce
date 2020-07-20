/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DopplerAudioProcessorEditor::DopplerAudioProcessorEditor(DopplerAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
{
    setSize (MAIN_PANEL_WIDTH, MAIN_PANEL_HEIGHT);

	// xy pad panel
	padxy = std::make_unique<PadXY>(p);
	addAndMakeVisible(*padxy); 
	padxy->setBounds(0, 0, XY_PAD_WIDTH, XY_PAD_HEIGHT);

	// parameter panel
	parampanel = std::make_unique<ParameterPanel>(p);
	addAndMakeVisible(*parampanel);
	parampanel->setBounds(BORDER_SIZE, XY_PAD_HEIGHT, PARAMETER_PANEL_WIDTH - (BORDER_SIZE * 2), PARAMETER_PANEL_HEIGHT - BORDER_SIZE);
}
 
void DopplerAudioProcessorEditor::paint(Graphics& g)
{
    //g.fillAll (Colours::white);
} 

void DopplerAudioProcessorEditor::resized()
{

}

DopplerAudioProcessorEditor::~DopplerAudioProcessorEditor()
{

}

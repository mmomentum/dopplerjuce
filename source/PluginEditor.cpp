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
	padxy->setBounds(BORDER_SIZE, BORDER_SIZE, XY_PAD_WIDTH - (BORDER_SIZE * 2), XY_PAD_HEIGHT - (BORDER_SIZE * 2));

	// parameter panel
	parampanel = std::make_unique<ParameterPanel>(p);
	addAndMakeVisible(*parampanel);
	parampanel->setBounds(BORDER_SIZE, XY_PAD_HEIGHT, PARAMETER_PANEL_WIDTH - (BORDER_SIZE * 2), PARAMETER_PANEL_HEIGHT - BORDER_SIZE);


	//((Component*)0)->getWidth();
	//padxy->addAndMakeVisible();
}
 
void DopplerAudioProcessorEditor::paint(Graphics& g)
{
    //g.fillAll (Colours::white);
} 

void DopplerAudioProcessorEditor::resized()
{
	//if(padxySafeYet)
		//padxy->setBounds (BORDER_SIZE, BORDER_SIZE, XY_PAD_WIDTH - (BORDER_SIZE*2), XY_PAD_HEIGHT - (BORDER_SIZE*2));
}

DopplerAudioProcessorEditor::~DopplerAudioProcessorEditor()
{
	//delete padxy;
	//padxy = 0;
}

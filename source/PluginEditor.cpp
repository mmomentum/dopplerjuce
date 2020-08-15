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
	parampanel->setBounds(BORDER_SIZE, XY_PAD_HEIGHT, PARAMETER_PANEL_WIDTH - (BORDER_SIZE * 2), PARAMETER_PANEL_HEIGHT);
}
 
void DopplerAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll (Colours::black);

	g.setGradientFill(juce::ColourGradient(backgroundColor, getWidth() / 10.0f, XY_PAD_HEIGHT,
		backgroundColor.darker(0.2), getWidth(), getHeight(), false));
	g.fillRoundedRectangle(0, XY_PAD_HEIGHT - 15, PARAMETER_PANEL_WIDTH, PARAMETER_PANEL_HEIGHT + 15, 10);

	g.setGradientFill(juce::ColourGradient(Colours::black, 0, XY_PAD_HEIGHT - (XY_PAD_HEIGHT * 0.06), Colours::transparentBlack, 0, XY_PAD_HEIGHT + 10, false));
	g.fillRect(0, 0, getWidth(), getHeight());


} 

void DopplerAudioProcessorEditor::resized()
{

}

DopplerAudioProcessorEditor::~DopplerAudioProcessorEditor()
{

}

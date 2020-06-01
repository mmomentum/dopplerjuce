/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "GlobalVariables.h"

#define X_ID "x"
#define X_NAME "x"

#define Y_ID "y"
#define Y_NAME "y"

#define SIZE_ID "size"
#define SIZE_NAME "size"

#define	SMOOTH_ID "smooth"
#define SMOOTH_NAME "smooth"

#define DISTANCE_ID "distance"
#define DISTANCE_NAME "distance"

//==============================================================================

class DopplerAudioProcessor : public AudioProcessor, public Timer
{
public:

	//==============================================================================
	DopplerAudioProcessor();
	~DopplerAudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	//==============================================================================
	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//==============================================================================

	void timerCallback() override;

	// calculate distance from origin for various processing systems
	float distanceCalculate(); 

	// calclate delay times (in samples) for IATD L / R delays (works using distanceCalculate())
	float delayCalculate(); 

	// calculate velocity (in meters / second) for FFT based doppler
	float velocityCalculate();

	AudioProcessorValueTreeState treeState;
	AudioProcessorValueTreeState::ParameterLayout createParameters();

private:
	//==============================================================================

	int globalSampleRate; // sets to whatever our current sample rate is

	// various physical values used by the audio processor systems
	float distance[2]; // calculated distance values (in meters)
	int delay[2]; // calculated delay values (in samples)
	float velocity[2]; // calculated velocity values (in m/s). positive values = moving towards you and vice versa.

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DopplerAudioProcessor)
};

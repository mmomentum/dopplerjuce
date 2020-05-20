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

class DopplerAudioProcessor : public AudioProcessor
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

	// custom function for copying input samples to a delay buffer (for IATD)
	void fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);
	
	// custom function for getting data from the buffer
	void getFromDelayBuffer(AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);

	//==============================================================================

	// calculate distance from origin for IATD
	float distanceCalculate(); 

	// calclate delay times (in samples) for L / R delays (works alongside distanceCalculate)
	float delayCalculate(); 

	AudioProcessorValueTreeState treeState;
	AudioProcessorValueTreeState::ParameterLayout createParameters();

private:

	AudioBuffer<float> mDelayBuffer; // for IATD
	int mWritePosition{ 0 }; // initial write position for delay
	int mSampleRate = getSampleRate();

	float distanceL, distanceR; // calculated distance values (in meters)

	int delayL, delayR; // calculated delay value (in samples)

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DopplerAudioProcessor)
};

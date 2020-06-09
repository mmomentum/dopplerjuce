/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "GlobalVariables.h"

#include "DelayBuffer.h"

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

class DopplerAudioProcessor : 
	public AudioProcessor, 
	public Timer
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
	float velocityCalculate(int channel);

	//==============================================================================

	AudioProcessorValueTreeState treeState;
	AudioProcessorValueTreeState::ParameterLayout createParameters();

private:

	float rateParam { 1.0f };

	float channelCountInv;

	Point<float> internalInterpolatorPoint = { 0,0 }; // internal interpolated point (and set to 0,0 to avoid errors)

	// various physical values used by the audio processor systems
	float distance[2]; // calculated distance values (in meters)
	int delaySamples[2]; // calculated delay values (in samples)
	float velocity[2]; // calculated velocity values (in m/s). positive values = moving towards you and vice versa.

	//==============================================================================

	//HDL::SimpleDelay<float> delay;
	//HDL::SimpleBiDelay<float> delay;
	//HDL::SpeedDelay<float> delay;
	HDL::SpeedMixDelay<float> delay;
	//HDL::AcceleratorDelay<float> delay;
	//HDL::MixDelay<float> delay;
	//HDL::MixBiDelay<float> delay;
	//HDL::MixBiSlewDelay<float> delay;
	//HDL::SineDelay<float> delay;
	//HDL::WobblyDelay<float> delay;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DopplerAudioProcessor)
};

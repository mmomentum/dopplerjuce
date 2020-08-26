/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "DryWet.h"

#include "GlobalVariables.h"
#include "ProcessorHelpers.h"

#include "ParameterDefines.h"

#define LEN 256

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
	float delayCalculate(int channel, int sampleRate);

	// calculate velocity (in meters / second) for FFT based doppler
	float velocityCalculate(int channel);

	void updateHRIRFilter();

	// calculate average of incoming block
	void getAverage(AudioBuffer<float> buffer, int blockSize, int blockChannels);

	//==============================================================================

	AudioProcessorValueTreeState treeState;
	AudioProcessorValueTreeState::ParameterLayout createParameters();

private:

	Point<float> internalInterpolatorPoint = { 0,0 }; // internal interpolated point (and set to 0,0 to avoid errors)

	static const int kChannels = 2;

	// various physical values used by the audio processor systems

	float distance[2]; // calculated distance values (in meters)
	float delaySamples[2]; // calculated delay values (in samples)
	float velocity[2]; // calculated velocity values (in m/s). positive values = moving towards you and vice versa.
	int theta = 72;

	std::array<double, 2> delayEffectValue;

	dsp::ProcessSpec spec;
	juce::AudioSampleBuffer monoBuffer;

	dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Lagrange3rd> delay { 88200 }; // max delay in samples (should probably adjust to change based on sample rate to save memory)

	dsp::FirstOrderTPTFilter<double> smoothFilter; // for smoothing of the filter time (also adjusted by the smoothing parameter)

	// cutoff filter stuff
	dsp::ProcessorDuplicator <dsp::IIR::Filter<float>, dsp::IIR::Coefficients <float>> lowPassFilter[2];

	// HRTF stuff
	dsp::FIR::Filter<float> IR_L;
	dsp::FIR::Filter<float> IR_R;

	// dry wet
	DryWetProcessor dryWet; // dry / wet mix system
	juce::AudioBuffer<float> dryBuffer; // for dry / wet mixing

	//==============================================================================

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DopplerAudioProcessor)
};
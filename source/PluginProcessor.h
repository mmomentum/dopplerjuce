/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "../libraries/r8brain-free-src/CDSPResampler.h"
#include "Delay.h"

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
	int delayCalculate(int channel, int sampleRate); 

	// calculate velocity (in meters / second) for FFT based doppler
	float velocityCalculate(int channel);

	void updateHRIRFilter();

	//==============================================================================

	AudioProcessorValueTreeState treeState;
	AudioProcessorValueTreeState::ParameterLayout createParameters();

private:

	Point<float> internalInterpolatorPoint = { 0,0 }; // internal interpolated point (and set to 0,0 to avoid errors)

	static const int kChannels = 2;

	// various physical values used by the audio processor systems

	float distance[2]; // calculated distance values (in meters)
	int delaySamples[2]; // calculated delay values (in samples)
	float velocity[2]; // calculated velocity values (in m/s). positive values = moving towards you and vice versa.
	int theta = 72;

	dsp::ProcessSpec spec;
	juce::AudioSampleBuffer monoBuffer;

	// resampling delay stuff (will probably just do a JUCE dsp::delayline once i figure out how)
	int DSP_buffer_size;
	double* buffer_of_doubles[kChannels];
	double* DSP_buffer[kChannels];
	OwnedArray<r8b::CDSPResampler< r8b::CDSPFracInterpolator< 6, 11 > >> upsampler;
	OwnedArray<r8b::CDSPResampler< r8b::CDSPFracInterpolator< 6, 11 > >> downsampler;
	Delay delay[kChannels]; // two delay processors

	// cutoff filter stuff
	dsp::ProcessorDuplicator <dsp::IIR::Filter<float>, dsp::IIR::Coefficients <float>> lowPassFilter[2];

	// HRTF stuff
	dsp::FIR::Filter<float> IR_L;
	dsp::FIR::Filter<float> IR_R;

	//==============================================================================

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DopplerAudioProcessor)
};
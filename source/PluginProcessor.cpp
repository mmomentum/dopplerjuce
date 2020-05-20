/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DopplerAudioProcessor::DopplerAudioProcessor() // constructor
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
		#if ! JucePlugin_IsMidiEffect
			#if ! JucePlugin_IsSynth
				.withInput("Input", AudioChannelSet::stereo(), true)
			#endif
			.withOutput("Output", AudioChannelSet::stereo(), true)
		#endif
	), treeState(*this, nullptr, "Parameters", createParameters())
#endif
{

}

//==============================================================================

const String DopplerAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool DopplerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool DopplerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool DopplerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double DopplerAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int DopplerAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int DopplerAudioProcessor::getCurrentProgram()
{
	return 0;
}

void DopplerAudioProcessor::setCurrentProgram(int index)
{
}

const String DopplerAudioProcessor::getProgramName(int index)
{
	return {};
}

void DopplerAudioProcessor::changeProgramName(int index, const String& newName)
{

}

//==============================================================================
void DopplerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	const int numInputChannels = getTotalNumInputChannels();
	const int delayBufferSamples = sampleRate;
	mSampleRate = sampleRate;

	mDelayBuffer.setSize(numInputChannels, delayBufferSamples);
	// max buffer size is one second of audio according to the sample rate, will adjust later for max memory efficiency
}

void DopplerAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DopplerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void DopplerAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...

	// define buffer sample lengths as constants
	const int bufferLength = buffer.getNumSamples();
	const int delayBufferLength = mDelayBuffer.getNumSamples();

	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		// create read pointers for delay buffers (essentially a "tape head" but digital and for memory)
		const float* bufferData = buffer.getReadPointer(channel);
		const float* delayBufferData = mDelayBuffer.getReadPointer(channel);

		fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData); // copy input buffer to delay buffer (circular buffer)
		getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
	}
	mWritePosition += bufferLength;
	mWritePosition %= delayBufferLength;
}

//==============================================================================

// custom functions for IATD used within the audio processor

void DopplerAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
	if (delayBufferLength > bufferLength + mWritePosition) // initial filling up of buffer
	{
		mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferLength, 0.8, 0.8);
	}
	else // after initial
	{
		const int bufferRemaining = delayBufferLength - mWritePosition;

		mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferRemaining, 0.8, 0.8);
		mDelayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength + bufferRemaining, 0.8, 0.8); // take last sample
	}
}

void DopplerAudioProcessor::getFromDelayBuffer(AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
	int delayTime = 600; // delay time in milliseconds (will simplify to samples later)
	const int readPosition = static_cast<int> (delayBufferLength + mWritePosition - (mSampleRate * delayTime / 1000)) % delayBufferLength;

	if (delayBufferLength > bufferLength + readPosition)
	{
		buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferLength);
	}
	else 
	{
		const int bufferRemaining = delayBufferLength - readPosition;
		buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
		buffer.addFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
	}
}

//==============================================================================
bool DopplerAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DopplerAudioProcessor::createEditor()
{
	DopplerAudioProcessorEditor *ret = new DopplerAudioProcessorEditor(*this);
	ret->padxy->getWidth();
	//ret->parampanel->getWidth();
	return ret;
}

//==============================================================================
void DopplerAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void DopplerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DopplerAudioProcessor();
}

// for our parameters. more parameters and types can be added relatively easily
AudioProcessorValueTreeState::ParameterLayout DopplerAudioProcessor::createParameters()
{
	std::vector <std::unique_ptr<RangedAudioParameter>> params; // vector of pointers for our parameters

	auto xPosParameter = std::make_unique<AudioParameterFloat>(X_ID, X_NAME, -1.0f, 1.0f, 0.0f); // define parameter
	params.push_back(std::move(xPosParameter)); // add to the params vector

	auto yPosParameter = std::make_unique<AudioParameterFloat>(Y_ID, Y_NAME, -1.0f, 1.0f, 0.0f);
	params.push_back(std::move(yPosParameter));

	auto sizeParameter = std::make_unique<AudioParameterFloat>(SIZE_ID, SIZE_NAME, 5.0f, 100.0f, 20.0f);
	params.push_back(std::move(sizeParameter));

	auto smoothParameter = std::make_unique<AudioParameterInt>(SMOOTH_ID, SMOOTH_NAME, 100, 5000, 500);
	params.push_back(std::move(smoothParameter));

	auto distanceParameter = std::make_unique<AudioParameterFloat>(DISTANCE_ID, DISTANCE_NAME, 5.0f, 50.0f, 9.0f);
	params.push_back(std::move(distanceParameter));

	return { params.begin(), params.end() };
}

float DopplerAudioProcessor::distanceCalculate()
{
	auto sizeValue = treeState.getRawParameterValue(SIZE_ID); // size of plane (meters)
	auto distanceValue = treeState.getRawParameterValue(DISTANCE_ID); // spacing distance between pickup points (meters)

	double distanceValueMeters = distanceValue[0] / 100.0; // divide down into centimeters once instead of four times per function call

	Point<float> soundEmitterLocationMeters; // point for the actual ACTUAL location (in meters instead of an arbitrary -1 - +1 range) 

	// multiply to the actual size
	soundEmitterLocationMeters.setXY((soundEmitterLocationXY.getX() * sizeValue[0]), (soundEmitterLocationXY.getY() * sizeValue[0]));

	// pythagorean theorem to get the distance values on our 2D plane
	distanceL = sqrt(square(distanceValueMeters - soundEmitterLocationMeters.getX()) + square(soundEmitterLocationMeters.getY()));
	distanceR = sqrt(square((distanceValueMeters * -1.0f) - soundEmitterLocationMeters.getX()) + square(soundEmitterLocationMeters.getY()));

	return 0;
}

float DopplerAudioProcessor::delayCalculate()
{
	// calculate delay times in samples by dividing distance by the speed
	// of sound in meters and then multiplying it by the current sample rate.

	delayL = roundToInt((distanceL / SPEED_OF_SOUND) * mSampleRate);
	delayR = roundToInt((distanceR / SPEED_OF_SOUND) * mSampleRate);

	return 0;
}

DopplerAudioProcessor::~DopplerAudioProcessor() // destructor
{

}

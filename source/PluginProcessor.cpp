/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

Point<float> soundEmitterLocationXY;

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
	), treeState(*this, nullptr, "Parameters", createParameters()), Timer()
#endif
{
	Timer::startTimerHz(60);
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
	delay.setChannelCount(getChannelCountOfBus(true, 0));
	channelCountInv = 1.f / float(getChannelCountOfBus(true, 0));
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
	// simple delay processing section (all of the heavy lifting is done in DelayBuffer.h)

	static int globalSampleRate = getSampleRate();

	distanceCalculate();
	
	int tempdelay = delayCalculate(0, globalSampleRate);

	delay.setDestination(delaySamples[0]);

	for (auto s = 0; s < buffer.getNumSamples(); ++s) {
		for (auto ch = 0; ch < buffer.getNumChannels(); ++ch) {
			auto& sample = *buffer.getWritePointer(ch, s);
			delay.write(sample, ch);
			sample += delay.readDelayValue(ch);
		}
		++delay;
	}
}

//==============================================================================
bool DopplerAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DopplerAudioProcessor::createEditor()
{
	return new DopplerAudioProcessorEditor(*this);
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

	double distanceValueMeters = distanceValue[0] / 100.0f; // divide down into centimeters once instead of four times per function call

	Point<float> soundEmitterLocationMeters; // point for the actual ACTUAL location (in meters instead of an arbitrary -1 - +1 range) 

	// multiply to the actual size
	soundEmitterLocationMeters.setXY((internalInterpolatorPoint.getX() * sizeValue[0]), (internalInterpolatorPoint.getY() * sizeValue[0]));

	// pythagorean theorem to get the distance values on our 2D plane
	distance[0] = sqrt(square(distanceValueMeters - soundEmitterLocationMeters.getX()) + square(soundEmitterLocationMeters.getY()));
	distance[1] = sqrt(square((distanceValueMeters * -1.0f) - soundEmitterLocationMeters.getX()) + square(soundEmitterLocationMeters.getY()));

	return 0;
}

int DopplerAudioProcessor::delayCalculate(int channel, int sampleRate)
{
	// calculate delay times in samples by dividing distance by the speed
	// of sound in meters and then multiplying it by the current sample rate.

	delaySamples[channel] = roundToInt((distance[channel] / SPEED_OF_SOUND) * sampleRate);

	return delaySamples[channel];
}

float DopplerAudioProcessor::velocityCalculate(int channel)
{
	// basically just calculating the difference between the current and last distances for L / R
	static float lastDistance[2] = { 0,0 };

	velocity[channel] = (distance[channel] - lastDistance[channel]) * 60.0f; // multiply by # of calls of timer per second
	lastDistance[channel] = distance[channel];

	return velocity[channel];
}

//==============================================================================

void DopplerAudioProcessor::timerCallback()
{
	// interpolation variables
	static Point<float> interpolationMovementAmount;
	static float interpolationTime;
	static float interpolationRemaining;

	// get X / Y coordinate parameters
	auto xCoordinate = treeState.getRawParameterValue(X_ID);
	auto yCoordinate = treeState.getRawParameterValue(Y_ID);

	// declare smoothing value
	auto smoothingValue = treeState.getRawParameterValue(SMOOTH_ID);

	// perform interpolation (nonlinear, *kind of* exponential)
	Point<float> diff;
	diff.setXY(xCoordinate[0] - (internalInterpolatorPoint.getX() - 0.0f), yCoordinate[0] - (internalInterpolatorPoint.getY() - 0.0f));

	float timePortion = getTimerInterval() / (smoothingValue[0] / 2); // smoothing value isn't actually in milliseconds i dont think...

	interpolationMovementAmount.setXY(diff.getX() * timePortion, diff.getY() * timePortion);
	internalInterpolatorPoint += interpolationMovementAmount;

	// set the global value to whatever the internal interpolated point is for this loop
	soundEmitterLocationXY.setXY(internalInterpolatorPoint.getX(), internalInterpolatorPoint.getY());

}

//==============================================================================

DopplerAudioProcessor::~DopplerAudioProcessor() // destructor
{
	Timer::stopTimer();
}
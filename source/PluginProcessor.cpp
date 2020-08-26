/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "HRIR3D.h"

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
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 1;

	

	IR_L.prepare(spec);
	IR_R.prepare(spec);
	updateHRIRFilter();
	monoBuffer.setSize(1, samplesPerBlock);
	IR_L.reset();
	IR_R.reset();

	spec.numChannels = 2;

	delay.prepare(spec);
	smoothFilter.prepare(spec); // filtering a value in this case

	delay.reset();
	smoothFilter.reset();

	dryWet.setDryWet(50.0f);
	dryWet.reset();
	dryBuffer.setSize(2, samplesPerBlock);
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
	// sliders
	auto minFreqParameter = treeState.getRawParameterValue(FILTER_ID);
	auto dopplerFactorParameter = treeState.getRawParameterValue(FACTOR_ID);
	auto dryWetParameter = treeState.getRawParameterValue(DRYWET_ID);

	auto smoothingParameter = treeState.getRawParameterValue(SMOOTH_ID);

	// buttons
	auto modeParameter = treeState.getRawParameterValue(MODE_ID); // delay or FFT mode
	auto volumeToggleParameter = treeState.getRawParameterValue(VOLUME_ID); // volume attenuation on / off
	auto dopplerToggleParameter = treeState.getRawParameterValue(DOPPLER_ID); // doppler shifting on / off
	auto hrtfToggleParameter = treeState.getRawParameterValue(HRTF_ID); // HRTF spatialization on / off

	// declare statics nescessary for processing audio once instead of running a
	// function to get them for every time they're needed (and each block cycle)

	static int globalSampleRate = getSampleRate();
	static int blockSize = buffer.getNumSamples();
	static int blockChannels = buffer.getNumChannels();

	// copy input to the dry buffer for later mixing
	dryBuffer.makeCopyOf(buffer, true);

	for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, blockSize);

	distanceCalculate(); // calculate distance from sound emitter to L / R listening points

	dsp::AudioBlock <float> block(buffer); // for some DSP stuff

	// delay process

	smoothFilter.setCutoffFrequency(0.1); // fix later so that the "smoothing" value can make the pitch go up and down more drastically with the speed of the dot

	if (dopplerToggleParameter[0] == 1)
	{
		for (int channel = 0; channel < blockChannels; channel++)
		{
			delayEffectValue[channel] = jlimit(0.0f, 44100.0f, dopplerFactorParameter[0] * delayCalculate(channel, globalSampleRate));

			for (int sample = 0; sample < blockSize; sample++)
			{
				auto delayValue = smoothFilter.processSample(int(channel), delayEffectValue[channel]);

				delay.pushSample(channel, buffer.getSample(channel, sample));
				delay.setDelay(float(delayValue));
				buffer.setSample(channel, sample, delay.popSample(channel));
			}
		}
	}

	// gain process

	if (volumeToggleParameter[0] == 1)
	{
		for (int channel = 0; channel < blockChannels; channel++) // per channel
		{
			auto* channelData = buffer.getWritePointer(channel);

			for (int sample = 0; sample < buffer.getNumSamples(); ++sample) // per sample
			{
				channelData[sample] *= gainCalculator(distance[channel]); // run gain calculator
			}
		}
	}

	// cutoff filter process

	//*lowPassFilter[0].state = *dsp::IIR::Coefficients<float>::makeLowPass(globalSampleRate, jmap(distance[0], 0.0f, float(minFreqParameter[0]), 22000.0f, 1500.0f), 1.0f);
	//*lowPassFilter[1].state = *dsp::IIR::Coefficients<float>::makeLowPass(globalSampleRate, jmap(distance[1], 0.0f, float(minFreqParameter[0]), 22000.0f, 1500.0f), 1.0f);
	//lowPassFilter[0].process(dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(0)));
	//lowPassFilter[1].process(dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(1)));

	//HRTF process 
	if (hrtfToggleParameter[0] == 1)
	{
		float angle = angleCalculator(soundEmitterLocationXY);

		//int elevation = 72;
		theta = angle / 15;

		updateHRIRFilter();

		for (int s = 0; s < blockSize; s++)
		{
			buffer.setSample(0, s, IR_L.processSample(buffer.getSample(0, s)));
			buffer.setSample(1, s, IR_R.processSample(buffer.getSample(1, s)));
		}
	}

	// dry wet process

	dryWet.setDryWet(dryWetParameter[0] / 100.0f);
	dryWet.processBlock(dryBuffer, buffer);
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
	// get state from current parameter tree and make it an XML
	ValueTree copyState = treeState.copyState();
	std::unique_ptr<XmlElement> xml = copyState.createXml();
	copyXmlToBinary(*xml.get(), destData);
}

void DopplerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// retrieve XML data and replace init parameters with the held ones
	std::unique_ptr<XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
	ValueTree copyState = ValueTree::fromXml(*xml.get());
	treeState.replaceState(copyState);
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

	auto dryWetParameter = std::make_unique<AudioParameterFloat>(DRYWET_ID, DRYWET_NAME, 0.0f, 100.0f, 100.0f);
	params.push_back(std::move(dryWetParameter));

	auto minFreqParameter = std::make_unique<AudioParameterFloat>(FILTER_ID, FILTER_NAME, NormalisableRange<float> {1500.0f, 22000.0f, 1.0f, std::log(0.5f) / std::log(980.0f / 19980.0f)}, 3000.0f);
	params.push_back(std::move(minFreqParameter));

	auto modeParameter = std::make_unique<AudioParameterBool>(MODE_ID, MODE_NAME, false);
	params.push_back(std::move(modeParameter));

	auto hrtfParameter = std::make_unique<AudioParameterBool>(HRTF_ID, HRTF_NAME, true);
	params.push_back(std::move(hrtfParameter));

	auto ampParameter = std::make_unique<AudioParameterBool>(VOLUME_ID, VOLUME_NAME, true);
	params.push_back(std::move(ampParameter));

	auto toggleDopplerParameter = std::make_unique<AudioParameterBool>(DOPPLER_ID, DOPPLER_NAME, true);
	params.push_back(std::move(toggleDopplerParameter));

	auto dopplerFactorParameter = std::make_unique<AudioParameterFloat>(FACTOR_ID, FACTOR_NAME, 0.0f, 3.0f, 1.0f);
	params.push_back(std::move(dopplerFactorParameter));

	return { params.begin(), params.end() };
}

float DopplerAudioProcessor::distanceCalculate()
{
	auto sizeValue = treeState.getRawParameterValue(SIZE_ID); // size of plane (meters)
	//auto distanceValue = treeState.getRawParameterValue(DISTANCE_ID); // spacing distance between pickup points (meters)

	//double distanceValueMeters = distanceValue[0] / 100.0f; // divide down into centimeters once instead of four times per function call

	double distanceValueMeters = 0.09; // divide down into centimeters once instead of four times per function call

	Point<float> soundEmitterLocationMeters; // point for the actual ACTUAL location (in meters instead of an arbitrary -1 - +1 range) 

	// multiply to the actual size
	soundEmitterLocationMeters.setXY((internalInterpolatorPoint.getX() * sizeValue[0]), (internalInterpolatorPoint.getY() * sizeValue[0]));

	// pythagorean theorem to get the distance values on our 2D plane
	distance[0] = sqrt(square(distanceValueMeters - soundEmitterLocationMeters.getX()) + square(soundEmitterLocationMeters.getY()));
	distance[1] = sqrt(square((distanceValueMeters * -1.0f) - soundEmitterLocationMeters.getX()) + square(soundEmitterLocationMeters.getY())); // inverted for Y a

	return 0;
}

float DopplerAudioProcessor::delayCalculate(int channel, int sampleRate)
{
	// calculate delay times in samples by dividing distance by the speed
	// of sound in meters and then multiplying it by the current sample rate.

	delaySamples[channel] = (distance[channel] / SPEED_OF_SOUND) * sampleRate;

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

void DopplerAudioProcessor::updateHRIRFilter()
{
	*(IR_L.coefficients) = dsp::FIR::Coefficients<float>(hrir_l[theta], LEN);
	*(IR_R.coefficients) = dsp::FIR::Coefficients<float>(hrir_r[theta], LEN);
}

void DopplerAudioProcessor::getAverage(AudioBuffer<float> buffer, int blockSize, int blockChannels)
{
	for (int channel = 0; channel < blockChannels; channel++)
	{
		float averageOfSamples = 0.0f;

		for (int sample = 0; sample < blockSize; ++sample)
			averageOfSamples += fabs(buffer.getSample(channel, sample));

		averageOfSamples /= blockSize;
		averageOfSamples /= blockChannels;
	}
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
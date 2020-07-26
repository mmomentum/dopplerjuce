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
	int TransitionBandInPercent = 4;
	int StopBandAttenuation = 100;
	int DSP_sample_rate = 192000; // need to go to a higher samplerate for resampling
	int maximum_buffer_size = samplesPerBlock * 1.25;

	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 1;

	IR_L.prepare(spec);
	IR_R.prepare(spec);
	updateHRIRFilter();
	monoBuffer.setSize(1, samplesPerBlock);
	IR_L.reset();
	IR_R.reset();

	for (int i = 0; i < getTotalNumOutputChannels(); i++)
	{
		buffer_of_doubles[i] = new double[maximum_buffer_size];  // the upsampler will overwrite this buffer and return it as the DSP buffer
		for (int j = 0; j < maximum_buffer_size; j++)
		{
			buffer_of_doubles[i][j] = 0;
		}
	}

	upsampler.clear();
	downsampler.clear();

	for (int channel = 0; channel < getTotalNumOutputChannels(); channel++)
	{
		upsampler.add(new r8b::CDSPResampler< r8b::CDSPFracInterpolator< 6, 11 > >(sampleRate, DSP_sample_rate, maximum_buffer_size, TransitionBandInPercent, StopBandAttenuation, r8b::EDSPFilterPhaseResponse(0), false));
		downsampler.add(new r8b::CDSPResampler< r8b::CDSPFracInterpolator< 6, 11 > >(DSP_sample_rate, sampleRate, maximum_buffer_size, TransitionBandInPercent, StopBandAttenuation, r8b::EDSPFilterPhaseResponse(0), false));
		delay[channel].prepareToPlay(sampleRate, sampleRate);
	}
}

void DopplerAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	for (int k = 0; k < upsampler.size(); ++k)
	{
		downsampler[k]->clear();
		upsampler[k]->clear();
	}
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
	// declare statics nescessary for processing audio once instead of running a
	// function to get them for every time they're needed (and each block cycle)

	static int globalSampleRate = getSampleRate();
	static int blockSize = buffer.getNumSamples();
	static int blockChannels = buffer.getNumChannels();

	for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, blockSize);

	distanceCalculate(); // calculate distance from sound emitter to L / R listening points

	dsp::AudioBlock <float> block(buffer);

	// delay process

	float** host_buffers = buffer.getArrayOfWritePointers();

	for (int channel = 0; channel < blockChannels; channel++)
	{
		delay[channel].setDelayTime(jlimit(0, 44100, delayCalculate(channel, globalSampleRate)));

		const float* float_sample = buffer.getReadPointer(channel);
		for (int sample = 0; sample < blockSize; ++sample)
		{
			buffer_of_doubles[channel][sample] = *float_sample;
			++float_sample;
		}

		// the upsamplers are called on each channel of the host buffers. DSP_buffer_size holds the new buffer size
		DSP_buffer_size = upsampler[channel]->process(buffer_of_doubles[channel], blockSize, DSP_buffer[channel]);

		// Delay Processor
		delay[channel].process(DSP_buffer[channel], DSP_buffer_size);

		// the downsamplers are called on each channel of the DSP buffers
		double* downsampler_output_sample;
		const int downsampler_buffer_size = downsampler[channel]->process(DSP_buffer[channel], DSP_buffer_size, downsampler_output_sample);

		// the downsampled signal is copied into the host buffers.
		// this step is necessary because the r8b library does not accept a buffer of floats
		for (int j = 0; j < jmin(blockSize, downsampler_buffer_size); ++j)
		{
			host_buffers[channel][j] = *downsampler_output_sample;
			++downsampler_output_sample;
		}
	}

	// attenuate / filter process


	// HRTF process 

	float angle = angleCalculator(soundEmitterLocationXY);

	int elevation = 72;
	theta = angle / 15 + elevation;

	updateHRIRFilter();

	IR_L.process(dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(0)));
	IR_R.process(dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(1)));
	//cutoffFilter.process(dsp::ProcessContextReplacing<float> (block));
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

	auto distanceParameter = std::make_unique<AudioParameterFloat>(DISTANCE_ID, DISTANCE_NAME, 5.0f, 100.0f, 9.0f);
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
	distance[1] = sqrt(square((distanceValueMeters * -1.0f) - soundEmitterLocationMeters.getX()) + square(soundEmitterLocationMeters.getY())); // inverted for Y a

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

void DopplerAudioProcessor::updateHRIRFilter()
{
	*(IR_L.coefficients) = dsp::FIR::Coefficients<float>(hrir_l[theta], LEN);
	*(IR_R.coefficients) = dsp::FIR::Coefficients<float>(hrir_r[theta], LEN);
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
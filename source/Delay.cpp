/*
  ==============================================================================

    Delay.cpp
    Created: 25 Jul 2020 3:45:43pm
    Author:  Aidan

  ==============================================================================
*/

#include "Delay.h"
#include <cmath>


#define TWOPI 6.28318530718

Delay::Delay()
{
	setDelayTime(250);
	setFeedbackGain(0.0f);
	setWowRate(1);
	setWowAmt(0.025);
	setFlutterRate(15);
	setFlutterAmt(0.025);
	setScrapeAmt(0.05);
	setBias(5);
	setIPS(3);

}

Delay::~Delay()
{

}

void Delay::prepareToPlay(int maxDelayTime, int samplerate)
{
	sample_rate = samplerate;
	osc_increment = OSCTABLESIZE / sample_rate;
	maximum_delay_time = maxDelayTime;

	// delay buffer is initialised
	delay_buffer_size = maximum_delay_time * sample_rate / 1000;
	delay_buffer = new float[delay_buffer_size]; // delay buffer is dynamically allocated before playback starts
	for (int i = 0; i < delay_buffer_size; i++)
	{
		delay_buffer[i] = 0; // delay buffer is initialised to 0
	}
	p_write = delay_buffer; //set pointer to delay buffer

	//set initial writing and reading positions
	writing_position = 0;
	reading_position = 0;

	// intantaneous delay time is initalised to 0 before playback
	// to recreate the progressive speed incrementation of the tape transport.
	// reading offset will increment until delay time target is reached.
	delay_time_current = 0;

	// fir buffer is initialised
	for (int i = 0; i < FIRBUFFERSIZE; i++)
	{
		fir_buffer[i] = 0;
	}
	fir_writing_position = 0;

}

void Delay::setDelayTime(float param_delay_time)
{
	delay_time_target = param_delay_time;
}

// main DSP function. processing is in place on the DSP_buffer pointer
// passed as first argument. buffer_size is the size of the DSP_buffer.
// Delay::initialise must be called before this function
void  Delay::process(double* DSP_buffer, int buffer_size)
{
	// this switch updates local variables and data structures
	// if the IPS or Bias paramters are changed by the user
	if (param_changed)
	{
		switch (IPS)
		{
		case 3:
			shaper_input_level = FASTIPSLEVEL;
			switch (bias)
			{
			case 2:
				p_fir_coefficients = FirTable::fir2;
				break;
			case 3:
				p_fir_coefficients = FirTable::fir4;
				break;
			case 4:
				p_fir_coefficients = FirTable::fir6;
				break;
			case 5:
				p_fir_coefficients = FirTable::fir8;
				break;
			}
			break;

		case 2:
			shaper_input_level = MEDIPSLEVEL;
			switch (bias)
			{
			case 2:
				p_fir_coefficients = FirTable::fir18;
				break;
			case 3:
				p_fir_coefficients = FirTable::fir20;
				break;
			case 4:
				p_fir_coefficients = FirTable::fir22;
				break;
			case 5:
				p_fir_coefficients = FirTable::fir24;
				break;
			}
			break;

		case 1:
			shaper_input_level = SLOWIPSLEVEL;
			switch (bias)
			{
			case 2:
				p_fir_coefficients = FirTable::fir34;
				break;
			case 3:
				p_fir_coefficients = FirTable::fir36;
				break;
			case 4:
				p_fir_coefficients = FirTable::fir38;
				break;
			case 5:
				p_fir_coefficients = FirTable::fir40;
				break;
			}
			break;
		}
		param_changed = false;
	}

	// main DSP loop. performs all DSP processing sample by sample,
	// computes the immediate reading and writing positions and performs
	// the DSP_buffer and delay_buffer reading and writing operations
	for (int i = 0; i < buffer_size; i++)
	{
		// inertia of tape transport, creates pitching effect when changing delay time
		// delay_time_current is set to reach delay_time_target over a period of time
		// delay_time_current speed towards delay_time_target is reduced as the
		// differential between them decreases
		delay_time_current = delay_time_current + 0.00001 * (delay_time_target - double(delay_time_current));

		// sample offset calculated from immediate delay time in milliseconds
		sample_offset = delay_time_current;// *sample_rate;

		// FIR filter is called on the DSP buffer and its ouput is written
		// into the delay buffer, according to the writing position
		p_write[writing_position] = firFilter(DSP_buffer[i]);

		// interpolateValue is called to read from the delay buffer.
		// its output is copied into the DSP buffer and the feedback tap
		DSP_buffer[i] = interpolateValue();
		feedback_sample = DSP_buffer[i];

		// writing position is incremented
		// and wrapped around the delay buffer
		writing_position++;
		if (writing_position >= delay_buffer_size)
		{
			writing_position = 0;
		}

		// reading position is calculated based on the writing position
		// and the sample offset. speed modulations are multiplied by their
		// maximum intensity in samples and added to the reading position
		reading_position = writing_position - sample_offset; //* wow_amt * wowOsc() + 50 * flutter_amt * flutterOsc() + 35 * scrape_amt * scrapeOsc();
		if (reading_position < 0)
		{
			reading_position += delay_buffer_size;
		}
	}
}

// interpolating function. takes closest values to the reading position
// and performs linear interpolation between them according to the instantanous reading position
float Delay::interpolateValue()
{
	int truncated_reading_position = (int)reading_position;
	float sample0 = delay_buffer[truncated_reading_position];
	float weight_sample1 = reading_position - truncated_reading_position;

	truncated_reading_position++;
	if (truncated_reading_position >= delay_buffer_size)
	{
		truncated_reading_position = 0;
	}

	float sample1 = delay_buffer[truncated_reading_position];
	return sample0 + weight_sample1 * (sample1 - sample0);
}

// FIR filter implementation using a circular buffer.
// input samples are passed using fir_input
float Delay::firFilter(float fir_input)
{
	// input sample is written into a circular buffer
	// and reading position is set to the last written sample
	fir_buffer[fir_writing_position] = fir_input;
	fir_output = 0;
	fir_reading_position = fir_writing_position;

	// output is calculated using multiply-add operations,
	// reading fir_buffer backwards and multiplying each sample
	// by its respective coefficient
	for (int i = 0; i <= FIRSIZE; i++)
	{
		fir_output += fir_buffer[fir_reading_position] * p_fir_coefficients[i];
		fir_reading_position--;
		fir_reading_position &= FIRSIZE;
	}
	// writing position is incremented and output is returned
	fir_writing_position++;
	fir_writing_position &= FIRSIZE;
	return fir_output;
}
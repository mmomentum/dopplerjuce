/*
  ==============================================================================

    Delay.h
    Created: 25 Jul 2020 3:45:43pm
    Author:  Aidan
    
    resampling delay taken from a project on github,
    will change to dsp::delayline once i iknow how
    to implement it...

  ==============================================================================
*/

#pragma once

#include "FirTable.h"

#define OSCTABLESIZE 4096
#define OSCTABLEMAX OSCTABLESIZE - 1
#define FIRSIZE ((1 << 7) - 1)
#define FIRBUFFERSIZE 128
#define SHAPERSIZE 32768
#define FASTIPSLEVEL 0.4
#define MEDIPSLEVEL 0.55
#define SLOWIPSLEVEL 0.8
#define NOISELENGTH ((1 << 22) - 1)
#define FASTIPSNOISELEVEL 0.006
#define MEDIPSNOISELEVEL 0.005
#define SLOWIPSNOISELEVEL 0.012
#define FEEDBACKTRIM 0.7


class Delay : public FirTable
{
public:
	Delay();
	~Delay();

	// parameter setting and getting functions
	void setDelayTime(float param_delay_time);
	float getDelayTime(void) { return delay_time_target * 1000; };
	void setFeedbackGain(float param_feedback_gain) { feedback_gain = param_feedback_gain * FEEDBACKTRIM; };
	float getFeedbackGain(void) { return feedback_gain; };
	void setWowRate(float param_wow_rate) { wow_rate = param_wow_rate; };
	float getWowRate(void) { return wow_rate; };
	void setWowAmt(float param_wow_amt) { wow_amt = param_wow_amt; };
	float getWowAmt(void) { return wow_amt; };
	void setFlutterRate(float param_flutter_rate) { flutter_rate = param_flutter_rate; };
	float getFlutterRate(void) { return flutter_rate; };
	void setFlutterAmt(float param_flutter_amt) { flutter_amt = param_flutter_amt; };
	float getFlutterAmt(void) { return flutter_amt; };
	void setScrapeAmt(float param_scrape_amt) { scrape_amt = param_scrape_amt; };
	float getScrapeAmt(void) { return scrape_amt; };
	void setBias(float param_bias)
	{
		if (bias != param_bias)
		{
			bias = (int)param_bias;
			param_changed = true;
		}
	};
	float getBias(void) { return (float)bias; };
	void setIPS(float param_IPS)
	{
		if (IPS != param_IPS)
		{
			IPS = (int)param_IPS;
			param_changed = true;
		}
	};
	float getIPS(void) { return (float)IPS; };

	// run in PrepareToPlay
	void prepareToPlay(int maxDelayTime, int samplerate);


	// run in ProcessBlock
	void process(double* DSP_buffer, int buffer_size);



private:

	float osc_table[OSCTABLESIZE];
	float scrape_table[OSCTABLESIZE];
	float wow_phi;
	float flutter_phi;
	float scrape_phi;
	int wow_truncated_phi;
	int flutter_truncated_phi;
	int scrape_truncated_phi;
	float wow_rate;
	float flutter_rate;
	float wow_amt;
	float flutter_amt;
	float scrape_amt;

	double sample_rate;
	float delay_time_target;    // this variable follows the delay time setting in the GUI
	float delay_time_current;   // instantaneous delay time
	float delay_time_initial;   // initial value of delay time when setDelayTime is called
	float* delay_buffer;
	int delay_buffer_size;

	float* p_write;            // writing pointer into the delay buffer
	float sample_offset;       // instantaneous delay time converted to fractional samples
	float reading_position;    // instantaneous fractional reading position
	int writing_position;       // instantaneous writing position. not fractional.
	float feedback_sample;     // feedback loop
	float feedback_gain;

	float interpolateValue();
	float firFilter(float fir_input);
	float fir_buffer[FIRBUFFERSIZE];
	int fir_writing_position;
	int fir_reading_position;
	//float fir_coefficients[FIRSIZE];
	float* p_fir_coefficients;
	float fir_output;
	int bias;
	int IPS;
	bool param_changed;
	float maximum_delay_time;

	float* p_shaper_table;
	int shaper_index;
	float shaper_x;

	float osc_increment;
	float shaper_input_level;
};

### TODO
#### Pitch-adjusting delay (tape-style delay)
- Implement a short, 100% wet stereo delay capable of adjusting the output signal based on the delay time. 
- The delay time has been pre-calculated with a function called delayCalculate(), be sure to include it in whatever routine you write.
- VCV rack's fundamental delay acts as we need a delay to act and it's source code can be found here: https://github.com/VCVRack/Fundamental/blob/v1/src/Delay.cpp
- a FFT based pitch shifter has also been worked out, but now we need to set up a function for calculating how much is supposed to *shift*  (in hz) rather than delay times.
#### Filtering
- Implement a low-pass filter which adjusts it's cutoff frequency based on the distance values for both left and right channels and thier corresponding distances. 
- The distance calculation has already been written for both channels in a function called distanceCalculate(), be sure to include it in whatever routine you write.
#### Attenuation
- Add an attenuator for adjusting the output level of the signal based on the distance for both left and right channels.
- The distance calculation has already been written for both channels in a function called distanceCalculate(), be sure to include it in whatever routine you write.
#### Ambisonics
- Work out ambisonics (which will handle panning / spatialization).
#### Value Smoothing
- Deal with the smoothing of values utilised in the audio processing block (and also per-sample processing)

~~As I suck my own dicque, I feel less lonely~~

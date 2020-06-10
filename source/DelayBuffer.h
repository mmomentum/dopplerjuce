#pragma once

namespace HDL {
	// // // basic functionality // // //

	template <typename Type>
	class Buffer {
	public:
		Buffer() :
			buffer(2)
		{
		}

		// write to buffer at idx
		void write(const Type& sample, const int& idx) { buffer[idx] = sample; }

		// read value from delay between i and idx
		const Type& read(int i, const int& idx) const {
			i = idx - i;
			if (i < 0)
				i += buffer.size();
			return buffer[i];
		}

		// read value from delay between i and idx (linear interpolation)
		const Type read(float i, const int& idx) const {
			i = idx - i;
			if (i < 0)
				i += buffer.size();
			auto fIdx = int(i);
			auto cIdx = (fIdx + 1) % buffer.size();
			auto frac = i - fIdx;

			return buffer[fIdx] + frac * (buffer[cIdx] - buffer[fIdx]);
		}

		// get vector of buffer
		std::vector<Type>& getVector() { return buffer; }

		// resize vector of buffer
		void resize(const int& s) { buffer.resize(s); }

		// get size of buffer
		const int& getSize() const { return buffer.size(); }

		// clear values from buffer
		void clear() {
			for (auto& i : buffer)
				i = 0;
		}
	private:
		std::vector<Type> buffer;
	};

	template <typename Type>
	class Delay {
	public:
		Delay() :
			size(44100),
			max(float(size - 1)),
			cIdx(0.f),
			dIdx(0.f),
			idx(0),
			buffer(1)
		{
			buffer[0].resize(size);
		}

		// write to buffer at idx
		void write(const Type& sample, const int& ch = 0) { buffer[ch].write(sample, idx); }

		// set channelCount
		void setChannelCount(const int& channelCount) {
			buffer.resize(channelCount);
			for (auto& i : buffer)
				i.resize(size);
		}

		// set destination delay (in samples)
		void setDestination(const float& delayInSamples) { dIdx = delayInSamples; }

		// set destination delay (normalized parameter)
		void setDestinationRelative(const float& d) { dIdx = d * max; }

		// get destination index of delay (in samples)
		const Type& getDestination() const { return dIdx; }

		// get current index of delay (in samples)
		const Type& getDelay() const { return cIdx; }

		// increment all delay indexes
		void operator ++() {
			follow();
			++idx %= size;
		}

		// method that contains rate change algorithm
		virtual void follow() = 0;

		// read value from delay between cIdx and i
		const Type& read(const int& i, const int& ch = 0) const { return buffer[ch].read(i, idx); }

		// read value from delay between cIdx and idx
		const Type& readDelayValue(const int& ch = 0) const { return buffer[ch].read(cIdx, idx); }

		// resize buffer
		void resize(const int& s) {
			size = s;
			max = float(size - 1);
			for (auto& i : buffer)
				i.resize(s);
		}

		// get size of buffer
		const int& getSize() const { return size; }

		// clear values from buffer
		void clear() {
			for (auto& i : buffer)
				i.clear();
		}

		int size; // buffer size
		float max; // max idx (size - 1)
		Type cIdx, dIdx; // current and destination indexes
	private:
		int idx; // index (writes to buffer)
		std::vector<Buffer<Type>> buffer; // audio buffer
	};

	// // // delay implementations // // //

	template <typename Type>
	class SimpleDelay : public Delay<Type> {
	public:
		SimpleDelay(const float& stepsI = 2.f) :
			steps(stepsI)
		{
		}

		void follow() override {
			if (std::abs(dIdx - cIdx) < steps) // ..delay moves in constant steps towards
				cIdx = dIdx;
			else if (cIdx < dIdx) // its destination..
				cIdx += steps;
			else if (cIdx > dIdx)
				cIdx -= steps;
		}
	private:
		float steps;
	};

	template <typename Type>
	class SimpleBiDelay : public Delay<Type> {
	public:
		SimpleBiDelay(const float& stepsU = 2.f, const float& stepsD = 1.f) :
			stepsUp(stepsU),
			stepsDown(stepsD), // ..delay behaves differently up/downwards
			minStep(jmin(stepsU, stepsD))
		{
		}

		void follow() override {
			if (std::abs(dIdx - cIdx) < minStep)
				cIdx = dIdx;
			else if (cIdx < dIdx)
				cIdx += stepsUp;
			else if (cIdx > dIdx)
				cIdx -= stepsDown;
		}
	private:
		float stepsUp, stepsDown, minStep;
	};

	template <typename Type>
	class SpeedDelay : public Delay<Type> {
	public:
		SpeedDelay(const float& speedI = 3.f) :
			maxInv(1.f / float(size - 1)),
			speed(speedI)
		{
		}

		void follow() override {
			auto distance = dIdx - cIdx; // distance to destination
			auto relDistance = distance * maxInv; // relative distance

			cIdx += relDistance * speed; // speed factor
		}

	private:
		float maxInv, speed;
	};

	template <typename Type>
	class SpeedBiDelay : public Delay<Type> {
	public:
		SpeedBiDelay(const float& speedU = 2.f, const float& speedD = 24.f) :
			maxInv(1.f / float(size - 1)),
			speedUp(speedU),
			speedDown(speedD)
		{
		}

		void follow() override {
			auto distance = dIdx - cIdx;
			auto relDistance = distance * maxInv;

			cIdx = distance < 0.f ?
				cIdx + relDistance * speedDown : // ..direction determines the
				cIdx + relDistance * speedUp; // speed..
		}

	private:
		float maxInv, speedUp, speedDown;
	};

	template <typename Type>
	class SpeedMixDelay : public Delay<Type> {
	public:
		SpeedMixDelay(const float& speedI = 3.f, const float& mixI = 1.0f) :
			maxInv(1.f / float(size - 1)),
			speed(speedI),
			mix(mixI),
			curSpeed(0.f),
			lastSpeed(0.f)
		{
		}

		void follow() override {
			auto relDistance = (dIdx - cIdx) * maxInv;
			curSpeed = lastSpeed + mix * (relDistance * speed - lastSpeed);
			cIdx += curSpeed;
			lastSpeed = curSpeed;
		}

	private:
		float maxInv, speed, mix, curSpeed, lastSpeed;
	};

	template <typename Type>
	class AcceleratorDelay : public Delay<Type> {
	public:
		AcceleratorDelay(const float& speedI = 4.f, const float& accelerationI = 1.5f) :
			maxInv(1.f / float(size - 1)),
			speed(speedI),
			acceleration(accelerationI)
		{
		}

		void follow() override {
			auto relDistance = (dIdx - cIdx) * maxInv;
			auto tri = relDistance < .5f ?
				2.f * relDistance :
				2.f - 2.f * relDistance; // slow at beginning and end of distance

			cIdx += std::tanh(acceleration * tri) * speed; // acceleration shape
			if (!(cIdx < size))
				cIdx = size - 1; // needs limitation (for some reason..)
		}

	private:
		float maxInv, speed, acceleration;
	};

	template <typename Type>
	class MixDelay : public Delay<Type> {
	public:
		MixDelay(const float& mixI = .0001f) :
			mix(mixI)
		{
		}

		// just the typical mix-algorithm..
		void follow() override { cIdx += mix * (dIdx - cIdx); }

	private:
		float mix;
	};

	template <typename Type>
	class MixBiDelay : public Delay<Type> {
	public:
		MixBiDelay(const float& mixU = .0001f, const float& mixD = .0005f) :
			mixUp(mixU),
			mixDown(mixD)
		{
		}

		void follow() override {
			auto distance = dIdx - cIdx;
			cIdx = distance < 0.f ?
				cIdx + mixUp * distance : // different speed / direction
				cIdx + mixDown * distance;
		}

	private:
		float mixUp, mixDown;
	};

	template <typename Type>
	class MixBiSlewDelay : public Delay<Type> {
	public:
		MixBiSlewDelay(const float& mixU = .0001f, const float& mixD = .0005f,
			const float& slewI = 20.f) :
			mixUp(mixU),
			mixDown(mixD),
			slew(slewI),
			curSpeed(0.f),
			lastSpeed(0.f)
		{
		}

		void follow() override {
			auto distance = dIdx - cIdx;
			curSpeed = distance < 0.f ?
				mixUp * distance : mixDown * distance;

			if (std::abs(curSpeed - lastSpeed) > slew)
				curSpeed = lastSpeed;

			cIdx += curSpeed;

			lastSpeed = curSpeed;
		}

	private:
		float mixUp, mixDown, slew, curSpeed, lastSpeed;
	};

	template <typename Type>
	class SineDelay : public Delay<Type> {
	public:
		SineDelay(const float& speedI = 2.f) :
			speed(speedI),
			maxInv(1.f / float(size - 1)),
			pi1p5(MathConstants<float>::pi * 1.5f)
		{
		}

		// use sine function on distance to smooth out movement
		void follow() override {
			auto relDistance = (dIdx - cIdx) * maxInv;
			auto transferCurve =
				std::sin(2.f * relDistance * MathConstants<float>::pi + pi1p5) * .5f + .5f;
			cIdx = relDistance > 0.f ?
				cIdx + transferCurve * speed :
				cIdx - transferCurve * speed;
		}

	private:
		float speed, maxInv, pi1p5;
	};

	template <typename Type>
	class WobblyDelay : public Delay<Type> {
	public:
		WobblyDelay(const float speedI = .2f) :
			speed(speedI),
			maxInv(1.f / float(size - 1)),
			saw(256)
		{
			auto maxInv = 1.f / 255.f; // preparing lookup table with wobbly saw wave
			auto normalizer = 0.f;
			for (auto i = 0; i < 256.f; ++i) { // as a transfer function
				auto phase = float(i) * maxInv;
				for (auto s = 1.f; s < 8.f; ++s) { // for the relative distance between the indexes
					saw[i] += std::sin(s * phase *
						MathConstants<float>::pi) / s;
				}
				auto absPhase = std::abs(saw[i]);
				if (normalizer < absPhase)
					normalizer = absPhase;
			}
			normalizer = 1.f / normalizer; // normalizing
			for (auto& i : saw)
				i *= normalizer;
		}

		void follow() override {
			auto relDistance = (dIdx - cIdx) * maxInv;

			auto tIdx = int(relDistance * speed * 255.f);
			if (relDistance < 0.f)
				cIdx -= saw[-tIdx];
			else
				cIdx += saw[tIdx];
		}

	private:
		float speed;
		float maxInv;
		std::vector<Type> saw;
	};
}
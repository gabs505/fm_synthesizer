/*
  ==============================================================================

	SynthAudioSource.h
	Created: 2 Dec 2018 2:47:15pm
	Author:  korowiow

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
struct SineWaveSound : public SynthesiserSound
{
	SineWaveSound() {}
	bool appliesToNote(int) override { return true; }
	bool appliesToChannel(int) override { return true; }
};

//==============================================================================
struct SineWaveVoice : public SynthesiserVoice
{
	
	SineWaveVoice(const AudioSampleBuffer& wavetableToUse, unsigned int voices) : wavetable(wavetableToUse)
	{
		globalLevel = 0.5 / (float)voices;
		jassert(wavetable.getNumChannels() == 1);
	}

	
	void setFrequency(float frequency, float sampleRate)
	{
		auto tableSizeOverSampleRate = wavetable.getNumSamples() / sampleRate;
		tableDelta = frequency * tableSizeOverSampleRate;
	}

	forcedinline float getNextSample() noexcept
	{
		auto tableSize = wavetable.getNumSamples();
		auto index0 = (unsigned int)currentIndex;
		auto index1 = index0 == (tableSize - 1) ? (unsigned int)0 : index0 + 1;
		auto frac = currentIndex - (float)index0;
		auto* table = wavetable.getReadPointer(0);
		auto value0 = table[index0];
		auto value1 = table[index1];
		auto currentSample = value0 + frac * (value1 - value0);
		if ((currentIndex += tableDelta) > tableSize)
			currentIndex -= tableSize;
		return currentSample;
	}
	




	
	bool canPlaySound(SynthesiserSound* sound) override
	{
		return dynamic_cast<SineWaveSound*> (sound) != nullptr;
	}
	void startNote(int midiNoteNumber, float velocity,
		SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
		
		currentIndex = 0.0;
		setFrequency(mod2freq, getSampleRate());
		
		currentAngle = 0.0;
		currentModAngle = 0.0;

		level = velocity * globalLevel;

		tailOn = 0.0;
		tailOff = 0.0;
		tailOffDecay = 1.0;
		auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		auto cyclesPerSample = cyclesPerSecond / getSampleRate();
		angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;

		auto cyclesPerSecondMod = cyclesPerSecond * (1 / modRatio);
		auto cyclesPerSampleMod = cyclesPerSecondMod / getSampleRate();
		angleModDelta = cyclesPerSampleMod * 2.0 * MathConstants<double>::pi;



	}
	void stopNote(float /*velocity*/, bool allowTailOff) override
	{
		if (allowTailOff) 
		{
			if (tailOff == 0.0)
				tailOff = 1.0;
		}
		else
		{
			clearCurrentNote();
			angleDelta = 0.0;
		}

	}
	void pitchWheelMoved(int) override {}
	void controllerMoved(int, int) override {}
	void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
	{
		if (angleDelta != 0.0) 
		{
			if (tailOff > 0.0) 
			{
				while (--numSamples >= 0)
				{
					auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);
					if (sustain != 0.0) {
						currentSample *= sustain;
					}


					auto currentModSample = (float)(std::sin(currentModAngle) * levelMod);
					auto mod2 = getNextSample() * mod2level;
					

					for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);
					currentAngle += angleDelta * (1 + currentModSample);

					currentModAngle += angleModDelta* (1 + mod2);

					++startSample;
					tailOff -= float(1 / (release * getSampleRate()));
					if (tailOff <= 0.005)
					{
						clearCurrentNote();
						angleDelta = 0.0;
						angleModDelta = 0.0;
						break;
					}
				}
			}
			else 
			{
				while (--numSamples >= 0)
				{
					auto currentModSample = (float)(std::sin(currentModAngle) * levelMod);
					
					auto mod2 = getNextSample() * mod2level;
					auto currentSample = (float)(std::sin(currentAngle) * level);

					
					if (attack != 0.0) {
						currentSample *= tailOn;
						if (tailOn <= 1) {

							tailOn += float(1 / (attack * getSampleRate()));
						}
						else if (decay != 0.0) {
							currentSample *= tailOffDecay;
							if (tailOffDecay > sustain) {
								tailOffDecay -= float((1 - sustain) / (decay * getSampleRate()));
							}


						}
						for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
							outputBuffer.addSample(i, startSample, currentSample);
						currentAngle += angleDelta * (1 + currentModSample);
						++startSample;

					}
					
					currentModAngle += angleModDelta * (1 + mod2);
				}
			}
		}
	}



	float attack = 0.2f, decay = 0.0f, sustain = 0.0f, release = 0.3f;
	float levelMod = 1.0f, modRatio = 1.0;
	float mod2level = 0.0f, mod2freq = 500.0f;
private:
	float tailOn = 0.0f;
	float tailOffDecay = 0.0f;
	float tailOff = 0.0; 

	

	
	double currentAngle = 0.0, angleDelta = 0.0;
	double currentModAngle = 0.0, angleModDelta = 0.0;
	float level = 0.0f, globalLevel = 0.0f;

	//wavetable synthesis of second modulator (saw)
	const AudioSampleBuffer& wavetable;
	float currentIndex = 0.0f, tableDelta = 0.0f;
};



//==============================================================================
class SynthAudioSource : public AudioSource
{
public:
	SynthAudioSource(MidiKeyboardState& keyState)
		: keyboardState(keyState)
	{
		for (auto i = 0; i < numVoices; ++i)
			voices.push_back(new SineWaveVoice(signalTable, numVoices));
		for (auto i = 0; i < numVoices; ++i)
			synth.addVoice(voices[i]);
		synth.addSound(new SineWaveSound());
	}

	void updateVoice(float a, float d, float s, float r) {
		for (auto i = 0; i < numVoices; ++i) {
			voices[i]->attack = a;
			voices[i]->decay = d;
			voices[i]->sustain = s;
			voices[i]->release = r;
		}
	}
	void updateVoiceFM(float modIdx, float modRat,float mod2amp,float mod2freq) {
		for (auto i = 0; i < numVoices; ++i) {
			voices[i]->levelMod = modIdx;
			voices[i]->modRatio = modRat;
			voices[i]->mod2level = mod2amp;
			voices[i]->mod2freq = mod2freq;

		}
	}
	void setUsingSineWaveSound()
	{
		synth.clearSounds();
	}
	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
	{
		synth.setCurrentPlaybackSampleRate(sampleRate);
		createWavetable();
	}
	void releaseResources() override {}
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		bufferToFill.clearActiveBufferRegion();
		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
		keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample,
			bufferToFill.numSamples, true);
		synth.renderNextBlock(*bufferToFill.buffer, incomingMidi,
			bufferToFill.startSample, bufferToFill.numSamples);
	}
	MidiMessageCollector* getMidiCollector()
	{
		return &midiCollector;
	}
	MidiMessageCollector midiCollector;

	std::vector< SineWaveVoice*>voices;



private:
	MidiKeyboardState& keyboardState;
	Synthesiser synth;

	// ***
	int tableSize = 128;
	int numVoices = 4;
	AudioSampleBuffer signalTable;
	void createWavetable()
	{
		signalTable.setSize(1, tableSize);
		auto* samples = signalTable.getWritePointer(0);
		
		auto currentValue = 1.0;
		for (auto i = 0; i < tableSize; ++i)
		{
		
			auto sample = currentValue;
			samples[i] = (float)sample;
			currentValue -= float(1.0/tableSize);

		}
	}


	
};

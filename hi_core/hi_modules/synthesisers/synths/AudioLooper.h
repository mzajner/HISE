/*  ===========================================================================
*
*   This file is part of HISE.
*   Copyright 2016 Christoph Hart
*
*   HISE is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   HISE is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with HISE.  If not, see <http://www.gnu.org/licenses/>.
*
*   Commercial licenses for using HISE in an closed source project are
*   available on request. Please visit the project's website to get more
*   information about commercial licensing:
*
*   http://www.hise.audio/
*
*   HISE is based on the JUCE library,
*   which must be separately licensed for closed source applications:
*
*   http://www.juce.com
*
*   ===========================================================================
*/

#ifndef AUDIOLOOPER_H_INCLUDED
#define AUDIOLOOPER_H_INCLUDED
namespace hise { using namespace juce;

class AudioLooper;

class AudioLooperSound : public ModulatorSynthSound
{
public:
	AudioLooperSound() {}

	bool appliesToNote(int /*midiNoteNumber*/) override   { return true; }
	bool appliesToChannel(int /*midiChannel*/) override   { return true; }
	bool appliesToVelocity(int /*midiChannel*/) override  { return true; }
};

class AudioLooperVoice : public ModulatorSynthVoice
{
public:

	AudioLooperVoice(ModulatorSynth *ownerSynth);;

	bool canPlaySound(SynthesiserSound *) override
	{
		return true;
	};

	void startNote(int midiNoteNumber, float /*velocity*/, SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

	void calculateBlock(int startSample, int numSamples) override;;
    
    // Added methods
    void setupLoopParameters(int& loopStart, int& loopEnd, int& actualLoopLength,
                             int& crossfadeLength, int& effectiveLoopLength,
                             int& length, bool& shouldCrossfade);
    
    void processTempoSyncedPlayback(int startSample, int samplesToCopy,
                                    const AudioSampleBuffer* buffer, int end);
    
    void processFreeRunningPlayback(int startIndex, int samplesToCopy,
                                    const float* leftSamples, const float* rightSamples,
                                    const AudioSampleBuffer* buffer, int loopStart,
                                    int actualLoopLength, int crossfadeLength,
                                    int effectiveLoopLength, int length,
                                    int loopOffset, bool isReversed, int end,
                                    bool shouldCrossfade, bool checkReset);
    
    void processSingleSample(int& startSample, int& numSamples, int uptime, double alpha,
                             const float* leftSamples, const float* rightSamples,
                             const AudioSampleBuffer* buffer, int loopStart,
                             int actualLoopLength, int crossfadeLength,
                             int effectiveLoopLength, int length,
                             int loopOffset, bool isReversed, int end,
                             bool shouldCrossfade, bool& resetAfterBlock, bool checkReset);
    
    void applyVoiceEffects(int startIndex, int samplesToCopy, bool isLastVoice,
                           int length, int loopOffset, bool isReversed);
	void resetVoice() override;
    
private:

	friend class AudioLooper;
    bool isFirstLoop = true;

    // Crossfade region indices relative to current loop
    int headStart = 0;
    int headEnd   = 0;
    int tailStart = 0;
    int tailEnd   = 0;
    
	time_stretcher stretcher;

	Random r;

};

/** A simple, one-file sample player with looping facitilies.
	@ingroup synthTypes

	Whenever you don't need a fully fledged streaming sampler, you can use
	this class to play a single sample that the user can change using
	a AudioDisplayWaveform component.
*/
class AudioLooper : public ModulatorSynth,
					public AudioSampleProcessor,
					public TempoListener,
					public MultiChannelAudioBuffer::Listener
{
public:

	ADD_DOCUMENTATION_WITH_BASECLASS(ModulatorSynth);

	SET_PROCESSOR_NAME("AudioLooper", "Audio Loop Player", "Plays a single audio sample.");

	enum SpecialParameters
	{
		SyncMode = ModulatorSynth::numModulatorSynthParameters, 
		LoopEnabled,
		PitchTracking,
		RootNote,
		SampleStartMod,
		Reversed,
        LoopCrossfade,
		numLooperParameters
	};

	AudioLooper(MainController *mc, const String &id, int numVoices);

	~AudioLooper() override;

	void restoreFromValueTree(const ValueTree &v) override;

	ValueTree exportAsValueTree() const override;

	void tempoChanged(double /*newTempo*/) override
	{
		setSyncMode(syncMode);
	};

	float getAttribute(int parameterIndex) const override;;

	float getDefaultValue(int parameterIndex) const override;

	void setInternalAttribute(int parameterIndex, float newValue) override;

	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		ModulatorSynth::prepareToPlay(sampleRate, samplesPerBlock);
		refreshSyncState();
	}

	void bufferWasLoaded() override;

	void bufferWasModified() override;

	ProcessorEditorBody* createEditor(ProcessorEditor *parentEditor) override;
	void setSyncMode(int newSyncMode);

	void setUseLoop(bool shouldBeEnabled)
	{
		loopEnabled = shouldBeEnabled;
	}

	bool isUsingLoop() const { return loopEnabled; }

	void refreshSyncState();
    
    void setCrossfadePercentage(float newCrossfadePercentage);
    
    void rangeChanged(AudioSampleBuffer* b, int areaIndex);
    void bufferReplaced(AudioSampleBuffer* b);

    void resetCrossfadeState();
private:

	HeapBlock<float> resampleBuffer;
	double resampleRatio;
	int numResampleBuffer;

	UpdateMerger inputMerger;


	bool loopEnabled = false;
	bool reversed = false;
	bool pitchTrackingEnabled;
	int rootNote;
    
    //Crossfade
    float crossfadePercentage = 0.0f;  // 0.0 to 1.0 (0% to 100%)
	int sampleStartMod = 0;
	friend class AudioLooperVoice;

	scriptnode::core::stretch_player<1>::tempo_syncer syncer;
	double numQuarters = 0.0;
	AudioSampleProcessor::SyncToHostMode syncMode;

};






} // namespace hise

#endif  // AUDIOLOOPER_H_INCLUDED

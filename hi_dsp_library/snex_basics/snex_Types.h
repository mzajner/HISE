#pragma once

namespace snex {
using namespace juce;

using FloatType = float;

#define SNEX_ENABLE_DEBUG_TYPENAMES 0

/** This will skip the string creation of type identifiers which takes up the most time in compiling in Debug mode. */
#ifndef SNEX_ENABLE_DEBUG_TYPENAMES
#define SNEX_ENABLE_DEBUG_TYPENAMES JUCE_DEBUG
#endif


#if SNEX_ENABLE_DEBUG_TYPENAMES
#define SNEX_TYPEDEBUG(x) x
#else
#define SNEX_TYPEDEBUG(x)
#endif

struct options
{
	static constexpr int no = 0;
	static constexpr int yes = 1;
	static constexpr int dynamic = 2;

	static constexpr bool isTrue(bool value, int optionValue)
	{
		if (optionValue == dynamic)
			return value;

		return static_cast<bool>(optionValue);
}

	static constexpr bool isTrueOrDynamic(int optionValue)
	{
		return optionValue == yes || optionValue == dynamic;
	}

	static constexpr bool isDynamic(int optionValue)
	{
		return optionValue == dynamic;
	}
};


struct CallbackTypes
{
	static constexpr int Channel = 0;
	static constexpr int Frame = 1;
	static constexpr int Sample = 2;
	static constexpr int NumCallbackTypes = 3;
	static constexpr int Inactive = -1;
};

namespace Types
{
enum ID
{
	Void =			0b00000000,
	Pointer =		0b10001111,
	Float =			0b00010000,
	Double =		0b00100000,
	Integer =		0b01000000,
	Block =			0b10000000,
	Dynamic =		0b11111111
};




template <typename T> ID getTypeFromTypeId()
{
    if (std::is_same<typename std::remove_pointer<T>::type, float>())
		return ID::Float;
	if (std::is_same<typename std::remove_pointer<T>::type, double>())
		return ID::Double;
	if (std::is_integral<T>() || std::is_same<typename std::remove_pointer<T>::type, int>())
		return ID::Integer;
	if (std::is_same<T, void*>())
		return ID::Pointer;

	return ID::Void;
}

/** This will identify each snex array type by using the constexpr static variable
    T::ArrayType
*/
enum class ArrayID
{
	SpanType,
	DynType,
	HeapType,
	ProcessDataType,
	FrameProcessorType
};

struct OutOfBoundsException
{
	OutOfBoundsException(int index_, int maxSize_) :
		index(index_),
		maxSize(maxSize_)
	{};

	int index;
	int maxSize;
};


struct FunctionType
{
	ID returnType;
	Identifier functionName;
	Array<ID> parameters;
};


namespace pimpl
{
	template <typename T> struct _ramp
	{
		using Type = _ramp<T>;

		/** Stops the ramping and sets the value to the target. */
		void reset()
		{
			stepsToDo = 0;
			value = targetValue;
			delta = T(0);
		}

		/** Sets a new target value and resets the ramp position to the beginning. */
		void set(T newTargetValue)
		{
			if (numSteps == 0)
			{
				targetValue = newTargetValue;
				reset();
			}
			else
			{
				auto d = newTargetValue - value;
				delta = d * stepDivider;
				targetValue = newTargetValue;
				stepsToDo = numSteps;
			}
		}

		/** Returns true if the value is being smoothed at the moment. */
		bool isActive() const
		{
			return stepsToDo > 0;
		}

		/** Returns the currently smoothed value and calculates the next ramp value. */
		T advance()
		{
			if (stepsToDo <= 0)
				return value;

			auto v = value;
			value += delta;
			stepsToDo--;

			return v;
		}

		/** Returns the current value. */
		T get() const
		{
			return value;
		}

		/** Setup the processing. The ramp time will be calculated based on the samplerate. */
		void prepare(double samplerate, double timeInMilliseconds)
		{
			if (samplerate > 0.0)
			{
				auto msPerSample = 1000.0 / samplerate;
				numSteps = roundToInt(timeInMilliseconds / msPerSample);

				if (numSteps > 0)
					stepDivider = T(1) / (T)numSteps;
				else
					stepDivider = T(0);
			}
			else
			{
				numSteps = 0;
				stepDivider = T(0);
			}
		}

		T value = T(0);
		T targetValue = T(0);
		T delta = T(0);
		T stepDivider = T(0);

		int numSteps = 0;
		int stepsToDo = 0;
	};
}


/** A smoothed float value.

	This object can be used to get a ramped value for parameter changes etc.


*/
struct sfloat : public pimpl::_ramp<float>
{};

struct sdouble : public pimpl::_ramp<double>
{};


/** A small helper class for usage within a wrap::mod node. 
*/
struct ModValue
{
	/** Return true if that changed flag was set. Use this in the 
	    handleModulation callback. */
	bool getChangedValue(double& d)
	{
		if (changed)
		{
			changed = false;
			d = (double)modValue;
			return true;
		}

		return false;
	}

	double getModValue() const noexcept { return (double)modValue; }

	/** Set the modulation value and the changed flag. This is best
	    used in a periodic context, where you don't care about the 
		change flag.
	*/
	void setModValue(double newValue)
	{
		modValue = (float)newValue;
		changed = true;
	}

	/** Set the modulation value and change flag only if the value has
	    changed. 

		If the modulation that you are using is not periodically, you 
		can use this in order to prevent unnecessary calls to the 
		modulation targets.
	*/
	bool setModValueIfChanged(double newValue)
	{
		if (modValue != (float)newValue)
		{
			modValue = (float)newValue;
			changed = true;
			return true;
		}

		return false;
	}

	void reset()
	{
		changed = false;
		modValue = 0.0f;
	}

	int changed = false;
	float modValue = 0.0f;
};

struct VoiceResetter
{
	virtual ~VoiceResetter() {};
	virtual void onVoiceReset(bool allVoices, int voiceIndex) = 0;
	virtual int getNumActiveVoices() const = 0;

	JUCE_DECLARE_WEAK_REFERENCEABLE(VoiceResetter);
};

struct DllBoundaryTempoSyncer: public hise::TempoListener
{
	typedef void(*MyFunc)(void*, double);

	

	struct Item
	{
		Item() :
			obj(nullptr),
			f(nullptr)
		{};

		Item(void* o, MyFunc f_) :
			obj(o),
			f(f_)
		{};

		bool operator==(const Item& other) const { return obj == other.obj; }
		operator bool() const { return obj != nullptr; }
		
		void call(double newTempo)
		{
			if (obj != nullptr)
				f(obj, newTempo);
		}

		void* obj = nullptr;
		MyFunc f = nullptr;
	};

	DllBoundaryTempoSyncer()
	{
		
	}

	~DllBoundaryTempoSyncer()
	{

	}

	/** Register an item that has a tempoChangedStatic class. */
	template <typename T> bool registerItem(T* obj)
	{
		Item newItem(obj, T::tempoChangedStatic);
		auto ok = data.insert(newItem);

		if(ok)
			newItem.call(bpm);

		return ok;
	}

	/** deregisters an item with the tempo changed class. */
	void deregisterItem(void* obj)
	{
		data.removeElement(Item(obj, nullptr));
	}

	void tempoChanged(double newTempo)
	{
		if (bpm != newTempo)
		{
			bpm = newTempo;

			for (auto& d : data)
				d.call(newTempo);
		}
	}

	double bpm = 120.0;
	hise::UnorderedStack<Item, 32> data;

	// Oh boy, what a disgrace...
	ModValue* publicModValue = nullptr;

	/** This can be used to temporarily change the pointer to the mod value.
		The OpaqueNetworkHolder uses this abomination of a class in the prepare
		call back in order to allow public_mod nodes to use the parent's network mod value.
	*/
	struct ScopedModValueChange
	{
		ScopedModValueChange(DllBoundaryTempoSyncer& d_, ModValue& m) :
			d(d_),
			prev(d.publicModValue)
		{
			d.publicModValue = &m;
		}

		~ScopedModValueChange()
		{
			d.publicModValue = prev;
		}

		DllBoundaryTempoSyncer& d;
		ModValue* prev;
	};
};

/** The PolyHandler can be used in order to create data structures that can be used
    in a polyphonic context.
	
	It assumes that the processing is being performed on a single thread and the voice
	index will always return -1 if it's being called from a different thread, even if the
	processing of a voice is being performed simultaneously.
	
	If you want to use a data type polyphonically, use the PolyData container and initialise
	it with an instance of the PolyHandler, then use the ScopedVoiceSetter before the processing
	and the PolyData container will only iterate over the active voice (while still being able
	to iterate over the entire container outside of this call. 

*/
struct PolyHandler
{
	/** Creates a poly handler. If you want to deactivate polyphonic behaviour altogether, pass in
	    false and it will bypass the entire threading and always return 0.
	*/
	PolyHandler(bool enabled_):
		enabled(enabled_ ? 1 : 0)
	{
		// This byte structure is needed by the JIT compilation, so if the atomic
		// wrappers add any overhead on a platform, this will fail compilation
		static_assert(offsetof(PolyHandler, enabled) == 12, "misaligned poly handler");
	}

	/** Call this whenever you change something from the UI. It will make sure that
	    the poly handler iteration will go through all voices as long as it's active.

		There is a minor performance overhead when the audio render thread is fetching
		the voice data because it has to check the thread ID against the thread that
		created this object.
	*/
	struct ScopedAllVoiceSetter
	{
		ScopedAllVoiceSetter(PolyHandler& p) :
			parent(p)
		{
			prevThread = parent.currentAllThread;
			parent.currentAllThread = Thread::getCurrentThreadId();
		}

		~ScopedAllVoiceSetter()
		{
			parent.currentAllThread = prevThread;
		}

		PolyHandler & parent;
		void* prevThread = nullptr;
	};

	/** Create an instance of this class with the given voice index and it will return this voice
	    index for each call that happens on this thread as long as this object exists.
	*/
	struct ScopedVoiceSetter
	{
		ScopedVoiceSetter(PolyHandler& p_, int voiceIndex) :
			p(p_)
		{
			if (p.enabled != 0)
			{
				jassert(p.currentAllThread != Thread::getCurrentThreadId());
				p.voiceIndex = voiceIndex;
			}
		}

		~ScopedVoiceSetter()
		{
			if (p.enabled != 0)
				p.voiceIndex = -1;
		}

	private:

		void* previousThread = nullptr;
		PolyHandler& p;
	};

	/** Returns the voice index. If its disabled, it will return always zero. If the thread is the processing thread
	    it will return the voice index that is being set with ScopedVoiceSetter, or -1 if it's called from another thread
		(or if the voice index has not been set). */
	int getVoiceIndex() const
	{
		if (currentAllThread != nullptr && Thread::getCurrentThreadId() == currentAllThread)
			return -1 * enabled;

		return voiceIndex.load() * enabled;
	}

	bool isEnabled() const { return enabled; }

	void setEnabled(bool shouldBeEnabled)
	{
		enabled = (int)shouldBeEnabled;

		if (shouldBeEnabled == 0)
		{
			currentAllThread = nullptr;
			voiceIndex.store(-1);
		}
	}

	static int getVoiceIndexStatic(PolyHandler* ph)
	{
		if (ph == nullptr)
			return 0;

		jassert(ph->enabled != 0);

		return ph->getVoiceIndex();
	}

	static int getSizeStatic(PolyHandler* ph)
	{
		if (ph == nullptr)
			return 0;

		jassert(ph->enabled != 0);

		if (ph->getVoiceIndex() == -1)
			return 1;

		return 0;
	}

	void sendVoiceResetMessage(bool allVoices)
	{
		if (vr.get() != nullptr)
			vr->onVoiceReset(allVoices, getVoiceIndex());
	}

	void setVoiceResetter(VoiceResetter* newVr)
	{
		vr = newVr;
	}

	VoiceResetter* getVoiceResetter() const
	{
		return vr.get();
	}

	void setTempoSyncer(DllBoundaryTempoSyncer* newTempoSyncer) { tempoSyncer = newTempoSyncer; }

	DllBoundaryTempoSyncer* getTempoSyncer() { return tempoSyncer; }

private:

	std::atomic<void*> currentAllThread = { nullptr }; // 0 byte offset
	std::atomic<int> voiceIndex = { -1 };			   // 8 byte offset
	int enabled;									   // 12 byte offset
	WeakReference<VoiceResetter> vr = nullptr;		   // 16 byte offset
	DllBoundaryTempoSyncer* tempoSyncer = nullptr;
};


/** A data structure containing the processing details for the context.

	This is being passed into the `prepare()` method of each node and
	can be used to setup the internals.

	*/
struct PrepareSpecs
{
	/** the sample rate. This value might be modified if the node is being used in an oversampled context. */
	double sampleRate = 0.0;

	/** The maximum samples that one block will contain. It is not guaranteed that the number will always be that amount, but you can assume that it never exceeds this number. */
	int blockSize = 0;

	/** The number of channels for the signal. */
	int numChannels = 0;

	PrepareSpecs withBlockSize(int newBlockSize) const
	{
		PrepareSpecs copy(*this);
		copy.blockSize = newBlockSize;
		return copy;
	}

	template <int BlockSize> PrepareSpecs withBlockSizeT() const
	{
		PrepareSpecs copy(*this);
		copy.blockSize = BlockSize;
		return copy;
	}

	operator bool() const
	{
		return numChannels > 0 && sampleRate > 0.0 && blockSize > 0;
	}

	PrepareSpecs withNumChannels(int newNumChannels) const
	{
		PrepareSpecs copy(*this);
		copy.numChannels = newNumChannels;
        return copy;
	}

	template <int NumChannels> PrepareSpecs withNumChannelsT() const
	{
		PrepareSpecs copy(*this);
		copy.numChannels = NumChannels;
		return copy;
	}

	/** A pointer to the voice index (see PolyData template). */
	PolyHandler* voiceIndex = nullptr;
};




/** A data structure that handles polyphonic voice data.

	In order to use it, create it (just like a span) and then
	use the range-based iterator to fetch the data.
	
	Depending on the context of the loop, it will either iterate over all values or 
	just pick the one for the currently active voice. In order for this to work, you 
	need to call prepare with a valid voice index pointer. 

	The element type T can be any class with a default constructor. For the sake of 
	optimizations, NumVoices has to be a number of two at all times. It's recommended
	to use either 1 or the preprocessor definition NUM_POLYPHONIC_VOICES, which offers
	a global way to set the max voice count per project.

	If the PolyData class is being used with NumVoices=1, the compiler should
	be able to remove the overhead of the class completely so you don't get
	any performance penalty by making your classes capable of handling polyphony!
*/
template <typename T, int NumVoices> struct PolyData
{
	PolyData(T initValue)
	{
		setAll(std::move(initValue));
	}

	PolyData()
	{
		if(std::is_arithmetic<T>::value)
			memset(data, 0, sizeof(T) * NumVoices);
	}

	/** Call this method with a PrepareSpecs objet and it will setup the handling of
	    the polyphony.
		
		It will use the int pointer in the PrepareSpecs object to figure out whether
		the voice rendering is enabled / active and what data slot to use.

		There are multiple states of the value at the sp.voiceIndex address:

		-1: the voice rendering is currently inactive (because it's called by the 
		    message thread or during initialization. Using the for loop now will
			result in an iteration over all elements.

		nullptr: the voice rendering is disabled. The class will act like PolyData<T, 1>,
		         (however it will still take the memory of NUM_VOICES elements).

		any number: the voice rendering is active and the for-loop will just iterate
		            once with the given number as offset from the data start.

	*/
	void prepare(const PrepareSpecs& sp)
	{
		jassert(!isPolyphonic() || sp.voiceIndex != nullptr);
		jassert(isPowerOfTwo(NumVoices));
		voicePtr = sp.voiceIndex;
	}

	void setAll(T&& value)
	{
		if (!isPolyphonic() || voicePtr == nullptr)
		{
			*data = std::move(value);
		}
		else
		{
			for (auto& d : *this)
				d = std::move(value);
		}
	}

	
	/** If you know that you're inside a rendering context, you can
	    use this function instead of the for-loop syntax. Be aware that
		the performance will be the same, it's just a bit less to type. 
	*/
	T& get() const
	{
		jassert(isMonophonicOrInsideVoiceRendering());
		return *begin();
	}

	/** Allows range-based for loops to work inside the voice context. */
	T* begin() const
	{
		if (isPolyphonic())
		{
			lastVoiceIndex = voicePtr != nullptr ? voicePtr->getVoiceIndex() : -1;
			auto startOffset = jmax(0, lastVoiceIndex);
			return const_cast<T*>(data) + startOffset;
		}
		else
			return const_cast<T*>(data);
	}

	T* end() const
	{
		if (isPolyphonic())
		{
			auto numToIterate = lastVoiceIndex == -1 ? NumVoices : 1;
			auto startOffset = jmax(0, lastVoiceIndex);
			return const_cast<T*>(data) + startOffset + numToIterate;
		}
		else
		{
			return const_cast<T*>(data) + 1;
		}
		
	}

	/** Just used during development. */
	String getVoiceIndexForDebugging() const
	{
#if JUCE_DEBUG
		String s;
		s << "VoiceIndex: ";
		s << (voicePtr != nullptr ? String(*voicePtr) : "inactive");
		return s;
#else
		return {};
#endif
	}

	/** Returns a reference to the first data. This can be used for UI purposes. */
	const T& getFirst() const
	{
		return *data;
	}

private:

	
	
	static constexpr bool isPolyphonic() { return NumVoices > 1; }

	bool isMonophonicOrInsideVoiceRendering() const
	{
		if (!isPolyphonic() || voicePtr == nullptr)
			return true;

		return isVoiceRenderingActive();
	}

	bool isVoiceRenderingActive() const
	{
		return isPolyphonic() &&
			voicePtr != nullptr && voicePtr->getVoiceIndex() != -1;
	}

	int getCurrentVoiceIndex() const
	{
		jassert(isPolyphonic());
		jassert(voicePtr != nullptr);
		return voicePtr->getVoiceIndex();
	}

	int getVoiceIndex(int index) const
	{
		auto rv = index & (NumVoices - 1);
		return rv;
	}


	T& getWithIndex(int index)
	{
		return *(data + getVoiceIndex(index));
	}

	const T& getWithIndex(int index) const
	{
		return *(data + getVoiceIndex(index));
	}

private:

	PolyHandler* voicePtr = nullptr;
	mutable int lastVoiceIndex = -1;
	int unused = 0;

	T data[NumVoices];
};

}



}

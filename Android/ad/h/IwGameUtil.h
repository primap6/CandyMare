// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#if !defined(_CIW_GAME_UTIL_H_)
#define _CIW_GAME_UTIL_H_

#include "s3e.h"
#include "IwList.h"

#ifndef NULL
    #define NULL 0
#endif

#define SAFE_DELETE(x) if (x != NULL) { delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) if (x != NULL) { delete [] x; x = NULL; }

#define	FRAME_SPEED_LOCK_MS		16.67f

//
//
// SINGLETONS
//
//
//
// Define a class as a singleton (Add to class definition in header file)
//
#define CDEFINE_SINGLETON(the_class)				\
private:										\
	static the_class* _instance;				\
	the_class() {}								\
	~the_class() {}								\
	the_class(const the_class &);				\
	the_class& operator=(const the_class &);	\
public:											\
	static void Create();						\
	static void Destroy();						\
	static the_class* getInstance();

//
// Declare singleton methods (Add to source file)
//
#define CDECLARE_SINGLETON(the_class)		\
the_class* the_class::_instance = NULL;		\
void the_class::Create()					\
{											\
	if (_instance == NULL)					\
		_instance = new the_class;			\
}											\
void the_class::Destroy()					\
{											\
	if (_instance != NULL)					\
	{										\
		delete _instance;					\
		_instance = NULL;					\
	}										\
}											\
the_class* the_class::getInstance()			\
{											\
  return _instance;							\
}

//
//
//
//
// IIwGameClassModifier - A class Modifier is attached to a class to modify its behvaiour
//
//
//
//
class IIwGameClassModifier
{
public:
	typedef CIwList<IIwGameClassModifier*>	ModifierList;
	typedef ModifierList::iterator			_ModifierIterator;

	// Properties
protected:
	unsigned int	ModiferNameHash;			// Hashed value of modifier name
	bool			ModifierActive;				// Modifiers active state
public:
	void			setModifierActive(bool active)	{ ModifierActive = active; }
	bool			getModifierActive() const		{ return ModifierActive; }
	unsigned int	getNameHash() const				{ return ModiferNameHash; }
	// Properties End
public:
	IIwGameClassModifier() : ModifierActive(true) {}
	virtual void	InitModfier() = 0;			// Initialise the Modifier
	virtual void	ReleaseModfier() = 0;		// Clean-up the Modifier
	virtual int		UpdateModfier() = 0;		// Update the Modifier
};


//
//
//
//
// CIwGameError - Game error logging class
//
//
//
//
class CIwGameError
{
public:
	static void LogError(const char* message);
	static void LogError(const char* message, const char* data);
};

//
//
//
//
// CIwGameCallback - Game callback type
//
//
//
//
typedef int32 (*CIwGameCallback)(void* caller, void* data);
typedef void* CIwGameCallbackData;

//
//
//
//
// CIwGameTimer (passive - needs to be polled with HasTimedOut)
//
//
//
//
class CIwGameTimer
{
protected:
	bool		Started;			// true if timer started
	uint64 		LastTime;			// Time that the timer was set (milliseconds)

	// Properties
private:
	uint64		Duration;			// Duration of timer in milliseconds
	bool		AutoReset;			// Auto reset the timer when it times out
public:
	void		setDuration(uint64 millseconds_duration, bool start = true)
	{
		Duration = millseconds_duration;
		if (start)
			Start();
	}
	uint64		getDuration() const				{ return Duration; }
	void		setAutoReset(bool auto_reset)	{ AutoReset = auto_reset; }
	bool		getAutoReset() const			{ return AutoReset; }
	bool		hasStarted() const				{ return Started; }
	bool		hasStopped() const				{ return !Started; }
	// Properties end

public:
	CIwGameTimer() : LastTime(0), Duration(0), AutoReset(false), Started(false) { }
	CIwGameTimer(int millseconds_duration)
	{
		LastTime = 0;
		Duration = 0;
		AutoReset = false;
		Started = false;
		setDuration(millseconds_duration);
	}
	~CIwGameTimer() {	}
	
	virtual bool HasTimedOut()
	{
		if (!Started)
			return false;

		if (Duration == 0)	// Timer of 0 duration never times out
		{
			return false;
		}
		
		uint64 time_diff = GetElapsedTime();
	
		if (time_diff > Duration)
		{
			if (AutoReset)
			{
				LastTime = s3eTimerGetMs();
			}
			else
				Started = false;
			return true;
		}
	
		return false;
	}
	uint64		GetElapsedTime() const
	{
		return s3eTimerGetMs() - LastTime;
	}
	uint64		GetTimeDiff(uint64 this_time) const { return this_time - LastTime; }
	uint64		GetTimeDiff() const					{ return s3eTimerGetMs() - LastTime; }
	uint64		GetTimeLeft() const					{ return Duration - (s3eTimerGetMs() - LastTime); }
	void		Reset()								{ setDuration(Duration); }
	void		Start()
	{
		LastTime = s3eTimerGetMs();
		Started = true;
	}
	void		Stop()								{ Started = false; }

	static uint64	GetCurrentTimeMs()				{ return s3eTimerGetMs(); }

};

//
//
//
// CIwGameUtils - Pure static utility class
//
//
//
class CIwGameUtils
{
public:
	static const char*	GetGraphicModeName(int width, int height);
	static int			CountDigits(int number);
};

//
//
//
// CIwGameArray -	Basic resizable slot array - A slot array differs from a normal array in that the array is not shrank and elements are not compatcted when elements are removed. Instead 
//					array elements that are removed are marked with NULL. With this, the Length of the array denotes the total number of items added to the array and not the number of valid 
//					elements. To get the number of valid elements call count()
//
//
//
template <class T>
class CIwGameSlotArray
{
	// Properties
protected:
	int		Growth;			// Number of elements to grow array when we run out of space
public:
	void	setGrowth(int growth)	{ Growth = growth; }
	int		getSize() const			{ return Size; }
	// Properties End
protected:
	T*		Elements;		// Array elements
	int		Size;			// Max size of array
	int		LastFreedSlot;	// Slot that was last freed

public:
	CIwGameSlotArray()
	{
		LastFreedSlot = 0;
		Growth = 4;
		Size = 4;
		Elements = new T[Size]();
		for (int t = 0; t < Size; t++)
			Elements[t] = NULL;
	}
	CIwGameSlotArray(int size, int growth)
	{
		LastFreedSlot = 0;
		Growth = growth;
		Size = size;
		Elements = new T[Size]();
	}
	
	~CIwGameSlotArray()
	{
		SAFE_DELETE(Elements)
	}

	int count() const
	{
		int count = 0;
		for (int t = 0; t < Size; t++)
		{
			if (Elements[t] != NULL)
				count++;
		}
		return count;
	}
	bool add(T element)
	{
		if (LastFreedSlot >= 0)
		{
			Elements[LastFreedSlot] = element;
			LastFreedSlot = -1;
			return true;
		}
		// find a free slot
		int t;
		for (t = 0; t < Size; t++)
		{
			if (Elements[t] == NULL)
			{
				Elements[t] = element;
				return true;
			}
		}

		// If we are not allowed to grow then return error
		if (Growth == 0)
			return false;

		// No free slots so grow the array
		grow(Growth);
		Elements[t] = element;
		LastFreedSlot = t + 1;

		return true;
	}

	void remove(T element)
	{
		for (int t = 0; t < Size; t++)
		{
			if (Elements[t] == element)
			{
				Elements[t] = NULL;
				LastFreedSlot = t;
				break;
			}
		}
	}

	void remove(int index)
	{
		if (index < 0 || index >= Size)
			return;
		Elements[index] = NULL;
		LastFreedSlot = index;
	}

	T element_at(int index) const
	{
		if (index < 0 || index >= Size)
			return NULL;
		return Elements[index];
	}

	void grow(int growth)
	{
		T* elements = new T[Size + growth]();
		for (int t = 0; t < Size; t++)
			elements[t] = Elements[t];
		for (int t = 0; t < growth; t++)
			elements[t + Size] = NULL;
		delete Elements;
		Elements = elements;
		Size += growth;
	}

	void clear()
	{
		for (int t = 0; t < Size; t++)
			Elements[t] = NULL;
		LastFreedSlot = 0;
	}

};


#endif	// _CIW_GAME_UTIL_H_

//----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
//----------------------------------------------------------------------------
// Timer 
//    v.2.1.0
//    v.2.5
//    v.2.6   - Baseline
//    v.2.6.1 - fixed warning TestRegistry
//    v.2.7   - Baseline
//    v.2.8   - warning 5039 windows.h
//    v.2.9   - fence
//    v.2.10  - msc_ver 1914, win 10.0.17
//    v.2.11  - warning fixes 
//    v.2.12  - mutex on all trace::out
//    v.2.13  - fixed warnings on mutex and default copy const / assignment
//    v.2.14  - FileIO remove new... so you can overload new/delete
//    v.2.15  - number tests pass reformat
//    v.2.16  - fixing static analysis issues
//    v.2.17  - missing deleted copy constructor and assignment operator in macro
//    v.2.18  - teardown issue
//    v.2.19  - WinSDK, Compiler, Framework version reporting
//    v.2.20  - WinSDK, Compiler, Framework version reporting in Release mode
//    v.2.21  - x64 info header
//---------------------------------------------------------------------------- 

#ifndef TIMER_H
#define TIMER_H

// Windows.h include
// many warnings - need to wrap for Wall warnings
#pragma warning( push )
	#pragma warning( disable : 4820 )
	#pragma warning( disable : 4668 )
	#pragma warning( disable : 5039 )
	#include <Windows.h>
#pragma warning( pop ) 

class Timer
{
public:
	// big four
	Timer() noexcept;
	Timer(const Timer &) = delete;
	Timer & operator= (const Timer &) = delete;
	~Timer() = default;

	// methods
	void Tic() noexcept;
	void Toc() noexcept;
	void Reset() noexcept;
	float TimeInSeconds() noexcept;

private:	
	// methods
	LARGE_INTEGER	privGetTimer() noexcept;
	void			privInitTimer() noexcept;

	// data
	LARGE_INTEGER	ticTime;
	LARGE_INTEGER	tocTime;
	LARGE_INTEGER	deltaTime;
	float			SecondsPerCycle;
	float			timeSeconds;

};

#endif TIMER_H

// ---  End of File ---------------

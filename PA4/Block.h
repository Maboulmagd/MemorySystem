//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef BLOCK_H
#define BLOCK_H

#include "Types.h"

enum class Block : uint8_t
{
	Free = 0xCC,
	Used = 0xAA
};

#endif

// ---  End of File ---------------

//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef USED_H
#define USED_H

#include "Types.h"
#include "Block.h"

class Free;

class Used
{
public:
	Used() = delete;
	Used(const Used&) = delete;
	Used& operator=(const Used&) = delete;
	~Used() = default;
	Used(const uint32_t block_size);

	Used       *pUsedNext;       // next used block
	Used       *pUsedPrev;       // prev used block
	uint32_t   mBlockSize;       // size of block
	Block      mType;            // block type 
	bool       mAboveBlockFree;  // AboveBlock flag
	                             //    if(AboveBlock is type free) -> true 
	                             //    if(AboveBlock is type used) -> false
	uint16_t    pad;             // future use
};

#endif 

// ---  End of File ---------------

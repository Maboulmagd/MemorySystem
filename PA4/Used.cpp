//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Free.h"
#include "Used.h"

Used::Used(const uint32_t block_size) : pUsedNext(nullptr), pUsedPrev(nullptr), mBlockSize(block_size), mType(Block::Used), mAboveBlockFree(false), pad(0)
{
}

// ---  End of File ---------------



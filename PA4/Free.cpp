//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Used.h"
#include "Free.h"
#include "Block.h"

Free::Free(const uint32_t block_size) : pFreeNext(nullptr), pFreePrev(nullptr), mBlockSize(block_size), mType(Block::Free), mAboveBlockFree(false)
{
}

// ---  End of File ---------------

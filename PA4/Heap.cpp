//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Heap.h"
#include "Mem.h"

Heap::Heap(void * ptr)
:	pUsedHead(0),
	pFreeHead(0),
	pNextFit(0)
{
	mStats.peakNumUsed = 0;			// number of peak used allocations
	mStats.peakUsedMemory  = 0;		// peak size of used memory

	mStats.currNumUsedBlocks = 0;	// number of current used allocations
	mStats.currUsedMem = 0;			// current size of the total used memory

	mStats.currNumFreeBlocks = 0;	// number of current free blocks
	mStats.currFreeMem = 0;			// current size of the total free memory

	mStats.heapTopAddr = reinterpret_cast<void *> ((uint8_t *)ptr + sizeof(Heap));		// start address available heap
	mStats.heapBottomAddr = reinterpret_cast<void *> ((uint8_t *)ptr + Mem::HEAP_SIZE);	// last address available heap

	mStats.sizeHeap = (uint32_t)mStats.heapBottomAddr - (uint32_t)mStats.heapTopAddr + sizeof(Heap); // size of Heap total space, including header
}

// ---  End of File ---------------

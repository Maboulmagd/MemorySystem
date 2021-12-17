//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef HEAPHDR_H
#define HEAPHDR_H

#include "Types.h"

#include "Used.h"
#include "Free.h"

class Heap
{
public:
	struct Stats
	{
		void *heapTopAddr;            // start address available heap
		void *heapBottomAddr;         // bottom of address of heap

		uint32_t currNumUsedBlocks;   // number of current used allocations
		uint32_t currUsedMem;         // current size of the total used memory
									  
		uint32_t peakNumUsed;         // number of peak used allocations
		uint32_t peakUsedMemory;      // peak size of used memory
									  
		uint32_t currNumFreeBlocks;   // number of current free blocks
		uint32_t currFreeMem;         // current size of the total free memory
									  
		uint32_t sizeHeap;            // size of Heap total space, including header
	};								  

public:
	// Make sure that the Heap is 16 byte aligned.

	// allocation links
	Used		*pUsedHead;
	Free		*pFreeHead;

	// Next fit allocation strategy
	Free		*pNextFit;	

	// stats
	Stats		mStats;

	// specialize constructor
	Heap(void * ptr);
};

#endif 

// ---  End of File ---------------

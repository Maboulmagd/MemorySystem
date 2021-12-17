//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include <malloc.h>
#include <new>

#include "Framework.h"

#include "Mem.h"
#include "Heap.h"
#include "Block.h"

#define STUB_PLEASE_REPLACE(x) (x)

// To help with coalescing... not required
struct SecretPtr
{
	Free *pFree;
};

void Mem::initialize() {
	// Add magic here
	//Trace::out("Size of Free*: %d\n", sizeof(Free*));
	//Trace::out("Size of Free: %d\n", sizeof(Free));
	//Trace::out("Size of Used: %d\n", sizeof(Used));
	//Trace::out("Size of uint32_t: %d\n", sizeof(uint32_t));
	//Trace::out("Size of Stats: %d\n", sizeof(Heap::Stats));
	//Trace::out("Size of Heap: %d\n", sizeof(Heap));

	Free* free_hdr_start = static_cast<Free*>(pHeap->mStats.heapTopAddr);
	Free* free_hdr_end = free_hdr_start + 1;

	const uint32_t heap_bottom_addr_int = reinterpret_cast<uint32_t>(pHeap->mStats.heapBottomAddr);
	const uint32_t free_hdr_end_int = reinterpret_cast<uint32_t>(free_hdr_end);

	const uint32_t block_size = heap_bottom_addr_int - free_hdr_end_int;

	// Construct Free block at 0x30 (48) with size
	pHeap->pFreeHead = placement_new(free_hdr_start, Free, block_size);
	pHeap->pNextFit = pHeap->pFreeHead;// Have the next fit ptr point to the start of the Free hdr

	// Update heap statistics
	pHeap->mStats.currNumFreeBlocks += 1;
	pHeap->mStats.currFreeMem += block_size;
}

void *Mem::malloc( const uint32_t size ) {
	STUB_PLEASE_REPLACE(size);
	return STUB_PLEASE_REPLACE(0);
}

void Mem::free( void * const data ) {
	STUB_PLEASE_REPLACE(data);	
}


// ---  End of File ---------------

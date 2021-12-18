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

void* Mem::malloc(const uint32_t size) {

	// We want to use the next fit ptr to get a block that can acommodate (>=) our size
	Free* next_fit = GetFreeBlock(size);
	if (next_fit != nullptr) {
		if (next_fit->mBlockSize == size) {// Case 1: perfect fit!

			Used* used_block = placement_new(next_fit, Used, size);
			InsertUsedBlock(used_block);

			// Update free head and next fit ptrs
			pHeap->pFreeHead = pHeap->pFreeHead->pFreeNext;

			next_fit = next_fit->pFreeNext;
			pHeap->pNextFit = next_fit;

			// Update heap statistics
			UpdateHeapStatisticsAfterMalloc(size);

			return reinterpret_cast<void*>(pHeap->pUsedHead + 1);
		}

		else if (next_fit->mBlockSize > size) {// Case 2: Free block has more space than required, will need to create a free block

		}
	}
	
	
	//if (next_fit == nullptr) {// When there are no more free blocks
	//	const uint32_t heap_top_addr_int = reinterpret_cast<uint32_t>(pHeap->mStats.heapTopAddr);
	//	// Set it to the top of the heap hdr
	//	pHeap->pNextFit = reinterpret_cast<Free*>(heap_top_addr_int - sizeof(Heap));
	//}

	return nullptr;
}

void Mem::free(void * const data) {

	STUB_PLEASE_REPLACE(data);	
}

// --- Private helper methods ---------------

Free* Mem::GetFreeBlock(const uint32_t block_size_required) {

	Free* next_fit = pHeap->pNextFit;

	Heap* top_of_heap = reinterpret_cast<Heap*>(reinterpret_cast<uint32_t>(pHeap->mStats.heapTopAddr) - sizeof(Heap));
	if (next_fit == reinterpret_cast<Free*>(top_of_heap)) {
		return nullptr;
	}

	//Free* heap_bottom_addr = static_cast<Free*>(pHeap->mStats.heapBottomAddr);

	while (next_fit != nullptr/* && next_fit != heap_bottom_addr*/) {
		if (next_fit->mBlockSize >= block_size_required) {// this block is available
			break;
		}
		next_fit = next_fit->pFreeNext;
	}

	return next_fit;
}

void Mem::InsertUsedBlock(Used* used_block_to_insert) {

	if (pHeap->pUsedHead == nullptr) {// Case 1: Very first allocation (used block)
		pHeap->pUsedHead = used_block_to_insert;
		return;
	}

	Used* next_used = pHeap->pUsedHead->pUsedNext;
	used_block_to_insert->pUsedNext = next_used;

	pHeap->pUsedHead = used_block_to_insert;
}

void Mem::UpdateHeapStatisticsAfterMalloc(const uint32_t size) {

	pHeap->mStats.currFreeMem -= size;
	pHeap->mStats.currNumFreeBlocks -= 1;

	pHeap->mStats.currUsedMem += size;
	pHeap->mStats.currNumUsedBlocks += 1;

	pHeap->mStats.peakUsedMemory = max(pHeap->mStats.peakUsedMemory, pHeap->mStats.currUsedMem);
	pHeap->mStats.peakNumUsed = max(pHeap->mStats.peakNumUsed, pHeap->mStats.currNumUsedBlocks);
}

void Mem::UpdateHeapStatisticsAfterFree(const uint32_t size) {

	pHeap->mStats.currFreeMem += size;
	pHeap->mStats.currNumFreeBlocks += 1;

	pHeap->mStats.currUsedMem -= size;
	pHeap->mStats.currNumUsedBlocks -= 1;

	//pHeap->mStats.peakUsedMemory = max(pHeap->mStats.peakUsedMemory, pHeap->mStats.currUsedMem);
	//pHeap->mStats.peakNumUsed = max(pHeap->mStats.peakNumUsed, pHeap->mStats.currNumUsedBlocks);
}

// ---  End of File ---------------

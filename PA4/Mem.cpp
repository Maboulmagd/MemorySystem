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
struct SecretPtr final {
	SecretPtr(Free* pFreeHdr)
		: pFree(pFreeHdr) {}

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

	// Place secret ptr
	Free* secret_ptr_addr = reinterpret_cast<Free*>(free_hdr_end_int + block_size - sizeof(SecretPtr));
	placement_new(secret_ptr_addr, SecretPtr, pHeap->pFreeHead);
	//Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Update heap statistics
	pHeap->mStats.currNumFreeBlocks += 1;
	pHeap->mStats.currFreeMem += block_size;
}

void* Mem::malloc(const uint32_t size) {

	// We want to use the next fit ptr to get a block that can acommodate (>=) our size
	Free* next_fit = GetFreeBlock(size);
	if (next_fit != nullptr) {
		if (next_fit->mBlockSize == size) {// Case 1: perfect fit!

			Free* next_free_block = next_fit->pFreeNext;

			RemoveFreeBlock(next_fit);// Update free block list

			Used* used_block = placement_new(next_fit, Used, size);
			InsertUsedBlock(used_block);

			// Update next fit
			next_fit = next_free_block;
			pHeap->pNextFit = next_fit;

			// Update heap statistics
			UpdateHeapStatisticsAfterPerfectMalloc(size);

			return reinterpret_cast<void*>(used_block + 1); // return the END of used header to the newly constructed used block.
		}

		else if (next_fit->mBlockSize > size) {// Case 2: Free block has more space than required, will need to create a free block

			// First, we get all the information we need to construct the free block after we construct the used block
			Free* new_free_block_hdr_start = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(next_fit + 1) + size);
			Free* new_free_block_hdr_end = new_free_block_hdr_start + 1;// This is of course also just the start of the free block
			const uint32_t new_free_block_size = next_fit->mBlockSize - size - sizeof(Free);// New free block's memory EXCLUDING its hdr

			// Now we can completely remove the free block from the free block list, SAFELY
			RemoveFreeBlock(next_fit);

			// Now, we can safely create the USED block, and insert it into the used block list
			Used* used_block = placement_new(next_fit, Used, size);
			InsertUsedBlock(used_block);

			// Now, we can safely create the FREE block, and insert it into the free block list, which is sorted in ascending order of memory address!
			Free* new_free_block = placement_new(new_free_block_hdr_start, Free, new_free_block_size);
			InsertFreeBlock(new_free_block);

			// Update next fit
			next_fit = new_free_block;
			pHeap->pNextFit = next_fit;

			// Let's also place the secret ptr for our newly created free block
			Free* new_free_block_end = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(new_free_block_hdr_end) + new_free_block_size);
			Free* secret_ptr_addr = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(new_free_block_end) - sizeof(SecretPtr));
			SecretPtr* secret_ptr = placement_new(secret_ptr_addr, SecretPtr, new_free_block);
			Trace::out("secret_ptr address: %p\n", secret_ptr);

			// Finally let's update the heap statistics, taking into account removal of free block, insertion of used block, and insertion of a free block!
			UpdateHeapStatisticsAfterPartialMalloc(size);

			return reinterpret_cast<void*>(used_block + 1);
		}
	}
	
	//if (next_fit == nullptr) {// When we've reached the end of the free block list
	//	const uint32_t heap_top_addr_int = reinterpret_cast<uint32_t>(pHeap->mStats.heapTopAddr);
	//	// Set it to the very top of the heap
	//	pHeap->pNextFit = reinterpret_cast<Free*>(heap_top_addr_int - sizeof(Heap));
	//}

	return nullptr;
}

void Mem::free(void * const data) {

	STUB_PLEASE_REPLACE(data);

	// TODO Before free'ing the USED block, we want to set the mAboveBlockFree flag to true for the block below (assuming THIS block isn't the last one).
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

	// Now just because next fit is at an end, it doesn't mean there are no free blocks that can accomodate our size.
	// It just means that we'll have to search from the beginning...so set next fit ptr to free head and start search again up until we hit the last search entry point.
	Free* last_addr_we_started_searching_from = pHeap->pNextFit;
	next_fit = pHeap->pFreeHead;
	while (next_fit != last_addr_we_started_searching_from) {
		if (next_fit->mBlockSize >= block_size_required) {// this block is available
			break;
		}
		next_fit = next_fit->pFreeNext;
	}

	return next_fit;
}

void Mem::RemoveFreeBlock(Free* free_block) {

	if (pHeap->pFreeHead == free_block) {// Case 1: block WAS the very first block in the free list
		pHeap->pFreeHead = pHeap->pFreeHead->pFreeNext;
		return;
	}

	// Fix ptr's for prev and next blocks
	if (free_block->pFreePrev != nullptr) {
		free_block->pFreePrev = free_block->pFreeNext;
	}

	if (free_block->pFreeNext != nullptr) {
		free_block->pFreeNext->pFreePrev = free_block->pFreePrev;
	}

	free_block->pFreeNext = nullptr;
	free_block->pFreePrev = nullptr;
}

void Mem::InsertUsedBlock(Used* used_block_to_insert) {

	if (pHeap->pUsedHead == nullptr) {// Case 1: Very first allocation (used block)
		pHeap->pUsedHead = used_block_to_insert;
		return;
	}

	used_block_to_insert->pUsedNext = pHeap->pUsedHead;
	pHeap->pUsedHead->pUsedPrev = used_block_to_insert;

	pHeap->pUsedHead = used_block_to_insert;
}

void Mem::UpdateHeapStatisticsAfterPerfectMalloc(const uint32_t malloc_size) {

	pHeap->mStats.currFreeMem -= malloc_size;
	pHeap->mStats.currNumFreeBlocks -= 1;

	pHeap->mStats.currUsedMem += malloc_size;
	pHeap->mStats.currNumUsedBlocks += 1;

	pHeap->mStats.peakUsedMemory = max(pHeap->mStats.peakUsedMemory, pHeap->mStats.currUsedMem);
	pHeap->mStats.peakNumUsed = max(pHeap->mStats.peakNumUsed, pHeap->mStats.currNumUsedBlocks);
}

void Mem::UpdateHeapStatisticsAfterPartialMalloc(const uint32_t malloc_size) {

	pHeap->mStats.currFreeMem -= malloc_size + sizeof(Free);
	// Well, currNumFreeBlocks shouldn't change, because we removed 1, and inserted another

	pHeap->mStats.currUsedMem += malloc_size;
	pHeap->mStats.currNumUsedBlocks += 1;// We did malloc a block

	pHeap->mStats.peakUsedMemory = max(pHeap->mStats.peakUsedMemory, pHeap->mStats.currUsedMem);
	pHeap->mStats.peakNumUsed = max(pHeap->mStats.peakNumUsed, pHeap->mStats.currNumUsedBlocks);
}

void Mem::UpdateHeapStatisticsAfterFree(const uint32_t free_size) {

	pHeap->mStats.currFreeMem += free_size;
	pHeap->mStats.currNumFreeBlocks += 1;

	pHeap->mStats.currUsedMem -= free_size;
	pHeap->mStats.currNumUsedBlocks -= 1;

	// Well, these shouldn't change, since we are FREE'ING memory...
	//pHeap->mStats.peakUsedMemory = max(pHeap->mStats.peakUsedMemory, pHeap->mStats.currUsedMem);
	//pHeap->mStats.peakNumUsed = max(pHeap->mStats.peakNumUsed, pHeap->mStats.currNumUsedBlocks);
}

void Mem::InsertFreeBlock(Free* free_block_to_insert) {

	Free* curr_free_block_hdr = pHeap->pFreeHead;

	if (curr_free_block_hdr == nullptr) {// Case 1: free_block is going to be the head of our list
		pHeap->pFreeHead = free_block_to_insert;
		return;
	}

	while (curr_free_block_hdr != nullptr) {
		if (free_block_to_insert < curr_free_block_hdr) {// We've found the block that's going to be the NEXT free block of the block we're inserting before it!

			// Fix ptrs
			Free* curr_free_block_hdr_prev = curr_free_block_hdr->pFreePrev;// Will be null when curr_free_block_hdr is the head of our free list!

			if (curr_free_block_hdr_prev != nullptr) {
				curr_free_block_hdr_prev->pFreeNext = free_block_to_insert;
			}
			free_block_to_insert->pFreePrev = curr_free_block_hdr_prev;// Will be null when curr_free_block_hdr is the head of our free list!

			curr_free_block_hdr->pFreePrev = free_block_to_insert;
			free_block_to_insert->pFreeNext = curr_free_block_hdr;

			// Edge Case: if curr_free_block_hdr is the head of our list, we need to update the head of our free list
			if (curr_free_block_hdr == pHeap->pFreeHead) {
				pHeap->pFreeHead = free_block_to_insert;
			}

			break;
		}

		// At this point, we haven't broken out of the while loop, which means the block we're inserting will be the last block in our free list!
		if (curr_free_block_hdr->pFreeNext == nullptr) {

			// Fix up ptrs
			curr_free_block_hdr->pFreeNext = free_block_to_insert;
			free_block_to_insert->pFreePrev = curr_free_block_hdr;

			break;
		}

		curr_free_block_hdr = curr_free_block_hdr->pFreeNext;
	}
}

// ---  End of File ---------------

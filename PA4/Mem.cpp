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

			// TODO Check if used_block is the head of the used list. If it is NOT, then update mAboveBlockFree flag for used_block...use the USED block list
			//		to determine whether there exists a previous USED block, and if so, use pointer arithmetic to check if it is directly above us,
			//		if it is NOT, then we KNOW FOR A FACT that the block directly above used_block MUST be a free block, and so set the mAboveBlockFlag to true.

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

			// TODO Check if used_block is the head of the used list. If it is NOT, then update mAboveBlockFree flag for used_block...use the USED block list
			//		to determine whether there exists a previous USED block, and if so, use pointer arithmetic to check if it is directly above us,
			//		if it is NOT, then we KNOW FOR A FACT that the block directly above used_block MUST be a free block, and so set the mAboveBlockFlag to true.

			// Now, we can safely create the FREE block, and insert it into the free block list, which is sorted in ascending order of memory address!
			Free* new_free_block = placement_new(new_free_block_hdr_start, Free, new_free_block_size);
			InsertFreeBlock(new_free_block);

			// NOTES: We know that the block directly above new_free_block is NOT a free block, so leave the mAboveBlockFlag as is (false).
			//		 We ALSO know that the block directly below this new free block (if we're not at the end of the heap) CANNOT be a free block, since 
			//		 the OLD free block that we just removed would've been coalesced with it!

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

	return nullptr;
}

void Mem::free(void * const data) {

	// NOTE Can't grab secret ptr from block above, UNLESS I know for sure that its a free block. To do that, we can remove the used block that we're required
	//		to remove anyway from our USED list. After that, we can create a free block in the exact same place, along with its secret ptr,
	//		and set next fit to point to it (ONLY if next fit is currently null), and insert the block in our free list. Don't forget to update heap stats. 
	//		ONLY than can we use our free list to determine whether there is a previous free block
	//		(if there isn't that means that we're the first one in the free list, AKA the freeHead). If there
	//		exists a previous free block, then we can use pointer arithmetic to determine whether it is directly above us,
	//		and then set the mAboveBlockFree flag of the newly created FREE block accordingly.
	//		As a sidenote, it is simple to check if the block below us is also a free block, just by using pointer arithmetic. Then we can SAFELY determine whether
	//		we need to coalesce with the block above, block below, both below and above, or none at all!

	//Used* used_block_hdr_end = reinterpret_cast<Used*>(data);
	Used* used_block_hdr_start = reinterpret_cast<Used*>(data) - 1;
	const uint32_t used_block_size = used_block_hdr_start->mBlockSize;// This will also just be the size of new_free_block

	// Now let's remove the used block from our used block list
	RemoveUsedBlock(used_block_hdr_start);

	// Now we can safely create our new free block to take the place of the used block we just removed, along with its secret ptr
	Free* new_free_block = placement_new(used_block_hdr_start, Free, used_block_size);
	Free* new_free_block_end = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(new_free_block + 1) + used_block_size);

	Free* secret_ptr_addr = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(new_free_block_end) - sizeof(SecretPtr));
	SecretPtr* secret_ptr = placement_new(secret_ptr_addr, SecretPtr, new_free_block);
	Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Update next fit, if need be
	if (pHeap->pNextFit == nullptr) {
		pHeap->pNextFit = new_free_block;
	}

	// Now let's insert the newly created free block in our free list
	InsertFreeBlock(new_free_block);

	// Now let's update our heap's statistics
	UpdateHeapStatisticsAfterFree(used_block_size);

	// Now we can check our free list to see if the prev free block, if there exists one (new_free_block isn't the head of our free list)
	// is directly above our newly created free block, and don't forget to update the mAboveBlockFree flag for prev_free_block
	bool coalesce_with_above_free_block = false;
	if (new_free_block != pHeap->pFreeHead) {
		Free* prev_free_block = new_free_block->pFreePrev;
		Free* prev_free_block_end = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(prev_free_block + 1) + prev_free_block->mBlockSize);
		
		if (prev_free_block_end == new_free_block) {
			coalesce_with_above_free_block = true;
			new_free_block->mAboveBlockFree = true;
		}
	}

	// Check if we also need to coalesce with the next free block, if there exists one (new_free_block isn't the tail of our free list) is directly below our newly
	// created free block, and dont forget to update the mAboveBlockFree flag for next_free_block
	bool coalesce_with_below_free_block = false;
	Free* next_free_block = new_free_block->pFreeNext;
	if (next_free_block != nullptr && next_free_block == new_free_block_end) {
		coalesce_with_below_free_block = true;
		next_free_block->mAboveBlockFree = true;
	}
	// If the block directly below new_free_block is a USED block, set its mAboveBlockFree flag to true
	else if (new_free_block_end < pHeap->mStats.heapBottomAddr) {
		Used* used_block_below_new_free_block = reinterpret_cast<Used*>(new_free_block_end);
		if (used_block_below_new_free_block->mType == Block::Used) {
			used_block_below_new_free_block->mAboveBlockFree = true;
		}
	}

	if (coalesce_with_above_free_block && coalesce_with_below_free_block) {
		CoalesceWithAboveAndBelowFreeBlocks(new_free_block->pFreePrev, new_free_block, new_free_block->pFreeNext);
	}

	else if (coalesce_with_above_free_block) {
		CoalesceWithAboveFreeBlock(new_free_block->pFreePrev, new_free_block);
	}

	else if (coalesce_with_below_free_block) {
		CoalesceWithBelowFreeBlock(new_free_block, new_free_block->pFreeNext);
	}
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

void Mem::RemoveFreeBlock(Free* free_block_to_remove) {

	if (pHeap->pFreeHead == free_block_to_remove) {// Case 1: block WAS the very first block in the free list
		pHeap->pFreeHead = pHeap->pFreeHead->pFreeNext;

		if (pHeap->pFreeHead != nullptr) {
			pHeap->pFreeHead->pFreePrev = nullptr;
		}

		return;
	}

	// Fix ptr's for prev and next blocks
	if (free_block_to_remove->pFreePrev != nullptr) {
		free_block_to_remove->pFreePrev->pFreeNext = free_block_to_remove->pFreeNext;
	}

	if (free_block_to_remove->pFreeNext != nullptr) {
		free_block_to_remove->pFreeNext->pFreePrev = free_block_to_remove->pFreePrev;
	}

	free_block_to_remove->pFreeNext = nullptr;
	free_block_to_remove->pFreePrev = nullptr;
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

void Mem::RemoveUsedBlock(Used* used_block_to_remove) {

	if (pHeap->pUsedHead == used_block_to_remove) {// Case 1: block WAS the very first block in the used list
		pHeap->pUsedHead = pHeap->pUsedHead->pUsedNext;

		if (pHeap->pUsedHead != nullptr) {
			pHeap->pUsedHead->pUsedPrev = nullptr;
		}

		return;
	}

	// Fix ptr's for prev and next blocks
	if (used_block_to_remove->pUsedPrev != nullptr) {
		used_block_to_remove->pUsedPrev->pUsedNext = used_block_to_remove->pUsedNext;
	}

	if (used_block_to_remove->pUsedNext != nullptr) {
		used_block_to_remove->pUsedNext->pUsedPrev = used_block_to_remove->pUsedPrev;
	}

	used_block_to_remove->pUsedNext = nullptr;
	used_block_to_remove->pUsedPrev = nullptr;
}

void Mem::CoalesceWithAboveAndBelowFreeBlocks(Free* prev_free_block, Free* new_free_block, Free* next_free_block) {

	// So the idea here is to remove all 3 free blocks from our FREE list, and just create 1 large free block in their place (just 1 FREE hdr)

	Free* new_large_free_block = prev_free_block;// The address of our new large free block will be here
	// The size of our new_large_free_block has to INCLUDE the size of 2 FREE hdrs, but EXCLUDE the size of its OWN FREE hdr
	const uint32_t new_large_free_block_size = prev_free_block->mBlockSize + new_free_block->mBlockSize + next_free_block->mBlockSize + (sizeof(Free) * 2);

	// Check if we'll need to update our next fit ptr to point to new_large_free_block at the end
	bool update_next_fit_ptr = false;
	if (pHeap->pNextFit == nullptr || pHeap->pNextFit == new_free_block) {
		update_next_fit_ptr = true;
	}

	// Now remove all 3 FREE blocks from our free list
	RemoveFreeBlock(prev_free_block);
	RemoveFreeBlock(new_free_block);
	RemoveFreeBlock(next_free_block);

	// Create the new large free block, along with its secret ptr and insert it
	new_large_free_block = placement_new(new_large_free_block, Free, new_large_free_block_size);

	Free* secret_ptr_addr = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr));
	SecretPtr* secret_ptr = placement_new(secret_ptr_addr, SecretPtr, new_large_free_block);
	Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Now let's insert new_large_free_block in our free list
	InsertFreeBlock(new_large_free_block);

	// Update the heap statistics!
	pHeap->mStats.currFreeMem += (sizeof(Free) * 2);// Think about it, currFreeMem will stay the same, but only increase in size since we REMOVED 2 FREE hdrs
	pHeap->mStats.currNumFreeBlocks -= 2;// We removed 3 free blocks, and then inserted 1 free block

	// Update the next fit ptr, if need be
	if (update_next_fit_ptr) {
		pHeap->pNextFit = new_large_free_block;
	}
}

void Mem::CoalesceWithAboveFreeBlock(Free* prev_free_block, Free* new_free_block) {

	// The idea here is to remove both FREE blocks from our FREE list, and just create 1 large free block in their place (just 1 FREE hdr)

	Free* new_large_free_block = prev_free_block;// The address of our new large free block will be here
	// The size of our new_large_free_block has to INCLUDE the size of 1 FREE hdr, but EXCLUDE the size of its OWN FREE hdr
	const uint32_t new_large_free_block_size = prev_free_block->mBlockSize + new_free_block->mBlockSize + sizeof(Free);

	// Check if we'll need to update our next fit ptr to point to new_large_free_block at the end
	bool update_next_fit_ptr = false;
	if (pHeap->pNextFit == nullptr || pHeap->pNextFit == new_free_block) {
		update_next_fit_ptr = true;
	}

	// Now remove prev_free_block and new_free_block from our free list
	RemoveFreeBlock(prev_free_block);
	RemoveFreeBlock(new_free_block);

	// Create the new large free block, along with its secret ptr
	new_large_free_block = placement_new(new_large_free_block, Free, new_large_free_block_size);

	Free* secret_ptr_addr = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr));
	SecretPtr* secret_ptr = placement_new(secret_ptr_addr, SecretPtr, new_large_free_block);
	Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Now let's insert new_large_free_block in our free list
	InsertFreeBlock(new_large_free_block);

	// Update the heap statistics!
	pHeap->mStats.currFreeMem += sizeof(Free);// Think about it, currFreeMem will stay the same, but only increase in size since we REMOVED 1 FREE hdr
	pHeap->mStats.currNumFreeBlocks -= 1;// We removed 2 free blocks, and then inserted 1 free block

	// Update the next fit ptr, if need be
	if (update_next_fit_ptr) {
		pHeap->pNextFit = new_large_free_block;
	}
}

void Mem::CoalesceWithBelowFreeBlock(Free* new_free_block, Free* next_free_block) {

	// The idea here is to remove both FREE blocks from our FREE list, and just create 1 large free block in their place (just 1 FREE hdr)
	
	Free* new_large_free_block = new_free_block;// The address of our new large free block will be here
	// The size of our new_large_free_block has to INCLUDE the size of 1 FREE hdr, but EXCLUDE the size of its OWN FREE hdr
	const uint32_t new_large_free_block_size = new_free_block->mBlockSize + next_free_block->mBlockSize + sizeof(Free);

	// Check if we'll need to update our next fit ptr to point to new_large_free_block at the end
	bool update_next_fit_ptr = false;
	if (pHeap->pNextFit == nullptr || pHeap->pNextFit == next_free_block) {
		update_next_fit_ptr = true;
	}

	// Now remove prev_free_block and new_free_block from our free list
	RemoveFreeBlock(new_free_block);
	RemoveFreeBlock(next_free_block);

	// Create the new large free block, along with its secret ptr
	new_large_free_block = placement_new(new_large_free_block, Free, new_large_free_block_size);

	Free* secret_ptr_addr = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr));
	SecretPtr* secret_ptr = placement_new(secret_ptr_addr, SecretPtr, new_large_free_block);
	Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Now let's insert new_large_free_block in our free list
	InsertFreeBlock(new_large_free_block);

	// Update the heap statistics!
	pHeap->mStats.currFreeMem += sizeof(Free);// Think about it, currFreeMem will stay the same, but only increase in size since we REMOVED 1 FREE hdr
	pHeap->mStats.currNumFreeBlocks -= 1;// We removed 2 free blocks, and then inserted 1 free block

	// Update the next fit ptr, if need be
	if (update_next_fit_ptr) {
		pHeap->pNextFit = new_large_free_block;
	}
}

// ---  End of File ---------------

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

	Free* const free_hdr_start = static_cast<Free*>(pHeap->mStats.heapTopAddr);
	//Free* free_hdr_end = free_hdr_start + 1;

	//const uintptr_t heap_bottom_addr_int = reinterpret_cast<uintptr_t>(pHeap->mStats.heapBottomAddr);
	const uintptr_t free_hdr_end_int = reinterpret_cast<uintptr_t>(free_hdr_start + 1);

	const uintptr_t block_size = reinterpret_cast<uintptr_t>(pHeap->mStats.heapBottomAddr) - free_hdr_end_int;

	// Construct Free block at 0x30 (48) with size
	pHeap->pFreeHead = placement_new(free_hdr_start, Free, block_size);
	pHeap->pNextFit = pHeap->pFreeHead;// Have the next fit ptr point to the start of the Free hdr

	// Place secret ptr
	//Free* secret_ptr_addr = reinterpret_cast<Free*>(free_hdr_end_int + block_size - sizeof(SecretPtr));
	placement_new(reinterpret_cast<Free*>(free_hdr_end_int + block_size - sizeof(SecretPtr)), SecretPtr, pHeap->pFreeHead);
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

			Free* const next_free_block = next_fit->pFreeNext;// Get next free block from our free list prior to removing the free block from our free list

			RemoveFreeBlock(next_fit);// Update free block list

			Used* const used_block = static_cast<Used*>(static_cast<void*>(next_fit));
			used_block->mType = Block::Used;
			InsertUsedBlock(used_block);

			// Update next fit
			next_fit = next_free_block;
			pHeap->pNextFit = next_fit;

			// Update heap statistics
			UpdateHeapStatisticsAfterPerfectMalloc(size);

			return static_cast<void*>(used_block + 1); // return the END of used header to the newly constructed used block.
		}

		else if (next_fit->mBlockSize > size) {// Case 2: Free block has more space than required, will need to create a free block

			// First, we get all the information we need to construct the free block after we construct the used block
			const uintptr_t new_free_block_hdr_start = reinterpret_cast<uintptr_t>(next_fit + 1) + size;
			//const uintptr_t new_free_block_hdr_end = new_free_block_hdr_start + sizeof(Free);// This is of course also just the start of the free block
			const uintptr_t new_free_block_size = next_fit->mBlockSize - size - sizeof(Free);// New free block's memory EXCLUDING its hdr

			Free* const prev_free_block = next_fit->pFreePrev;
			Free* const next_free_block = next_fit->pFreeNext;

			// Now we can remove the free block from the free block list
			RemoveFreeBlock(next_fit);

			// Now, we can "make" a USED block, and insert it into the used block list
			Used* const used_block = static_cast<Used*>(static_cast<void*>(next_fit));
			used_block->mBlockSize = size;
			used_block->mType = Block::Used;
			InsertUsedBlock(used_block);

			// Now, we can safely create the FREE block, and insert it into the free block list, which is sorted in ascending order of memory address!
			Free* const new_free_block = placement_new(reinterpret_cast<Free*>(new_free_block_hdr_start), Free, new_free_block_size);
			new_free_block->pFreePrev = prev_free_block;
			if (prev_free_block != nullptr) {
				prev_free_block->pFreeNext = new_free_block;
			}
			else {
				pHeap->pFreeHead = new_free_block;
			}
			new_free_block->pFreeNext = next_free_block;
			if (next_free_block != nullptr) {
				next_free_block->pFreePrev = new_free_block;
			}

			// NOTE: We know that the block directly above new_free_block is NOT a free block, so leave the mAboveBlockFlag as is (false).
			//		 We ALSO know that the block directly below this new free block (if we're not at the end of the heap) CANNOT be a free block, since 
			//		 the OLD free block that we just removed would've been coalesced with it!

			// Update next fit
			next_fit = new_free_block;
			pHeap->pNextFit = next_fit;

			// Let's also place the secret ptr for our newly created free block
			//const uintptr_t new_free_block_end = new_free_block_hdr_end + new_free_block_size;
			//Free* const secret_ptr_addr = reinterpret_cast<Free*>(new_free_block_end - sizeof(SecretPtr));
			placement_new(reinterpret_cast<Free*>(((new_free_block_hdr_start + sizeof(Free)) + new_free_block_size) - sizeof(SecretPtr)), SecretPtr, new_free_block);
			//Trace::out("secret_ptr address: %p\n", secret_ptr);

			// Finally let's update the heap statistics, taking into account removal of free block, insertion of used block, and insertion of a free block!
			UpdateHeapStatisticsAfterPartialMalloc(size);

			return static_cast<void*>(used_block + 1);
		}
	}

	return nullptr;
}

void Mem::free(void * const data) {

	// NOTE Can't grab secret ptr from block above, UNLESS I know for sure that its a free block. To do that, we can remove the used block that we're required
	//		to remove anyway from our USED list, but BEFORE doing so, if our used block's mAboveBlockFree flag was set to TRUE when we free'd the block above us,
	//		then we can very simply grab the secret ptr above us and then get hold of the free block above us.
	//		As a sidenote, it is simple to check if the block below us is also a free block, just by using pointer arithmetic. Then we can SAFELY determine whether
	//		we need to coalesce with the block above, block below, both below and above, or none at all!

	Used* const used_block_hdr_start = reinterpret_cast<Used*>(data) - 1;
	//Used* used_block_hdr_end = reinterpret_cast<Used*>(data);
	const uint32_t used_block_size = used_block_hdr_start->mBlockSize;// This will also just be the size of new_free_block

	// Try to see if we are adjacent to any FREE blocks (either above, below, or maybe even both)! This is to try to avoid looping to insert the free block that we will create instead of the used block that we're free'ing
	Free* free_block_above_used = nullptr;
	bool block_above_is_free = false;
	if (used_block_hdr_start->mAboveBlockFree) {
		block_above_is_free = true;
		SecretPtr* secret_ptr_for_above_block = reinterpret_cast<SecretPtr*>(reinterpret_cast<uint32_t>(used_block_hdr_start) - sizeof(SecretPtr));
		free_block_above_used = secret_ptr_for_above_block->pFree;
	}
	
	// I am just using Free* here, but I could've used Used*...
	Free* free_block_below_used = reinterpret_cast<Free*>(reinterpret_cast<uint32_t>(data) + used_block_size);
	bool block_below_is_free = false;
	if (free_block_below_used < pHeap->mStats.heapBottomAddr) {
		if (free_block_below_used->mType == Block::Free) {
			block_below_is_free = true;
			// NOTE Could set mAboveBlockFree flag here for free_block_below_used to true, but its useless, since we'll be coalescing with it anyways!
		}
		else if (free_block_below_used->mType == Block::Used) {
			free_block_below_used->mAboveBlockFree = true;
		}
	}
	
	// Now let's remove the used block from our used block list
	RemoveUsedBlock(used_block_hdr_start);

	// Now let's update our heap's statistics
	pHeap->mStats.currUsedMem -= used_block_size;
	pHeap->mStats.currNumUsedBlocks -= 1;

	// Now let's insert the newly created free block in our free list
	if (!block_above_is_free && !block_below_is_free) {
		// Now we can safely create our new free block to take the place of the used block we just removed, along with its secret ptr
		Free* new_free_block = placement_new(used_block_hdr_start, Free, used_block_size);
		//const uintptr_t new_free_block_end = reinterpret_cast<uintptr_t>(new_free_block + 1) + used_block_size;

		//Free* secret_ptr_addr = reinterpret_cast<Free*>(new_free_block_end - sizeof(SecretPtr));
		placement_new(reinterpret_cast<Free*>(reinterpret_cast<uintptr_t>(new_free_block + 1) + used_block_size - sizeof(SecretPtr)), SecretPtr, new_free_block);
		//Trace::out("secret_ptr address: %p\n", secret_ptr);

		// Update next fit, if need be
		if (pHeap->pNextFit == nullptr) {
			pHeap->pNextFit = new_free_block;
		}

		// No option but to insert our free block in the free sorted list via looping
		InsertFreeBlock(new_free_block);

		// Now let's update our heap's statistics
		pHeap->mStats.currFreeMem += used_block_size;// Think about it, currFreeMem will gain the size of the used block we just removed
		pHeap->mStats.currNumFreeBlocks += 1;// We inserted 1 free block
	}

	else if (block_above_is_free && block_below_is_free) {
		CoalesceWithAboveAndBelowFreeBlocks(free_block_above_used, reinterpret_cast<Free*>(used_block_hdr_start), free_block_below_used);

		// Now let's update our heap's statistics
		pHeap->mStats.currFreeMem += used_block_size + (sizeof(Free) * 2);// We removed 1 free block, and the 1 used block, so we add sizes of the headers, and we also add used_block_size
		pHeap->mStats.currNumFreeBlocks -= 1;// We removed free_block_below_used
	}

	else if (block_above_is_free) {
		CoalesceWithAboveFreeBlock(free_block_above_used, reinterpret_cast<Free*>(used_block_hdr_start));

		// Now let's update our heap's statistics
		pHeap->mStats.currFreeMem += used_block_size + sizeof(Used);// We add the 1 used block's header size, and used_block_size
	}

	else if (block_below_is_free) {
		CoalesceWithBelowFreeBlock(reinterpret_cast<Free*>(used_block_hdr_start), free_block_below_used);

		// Now let's update our heap's statistics
		pHeap->mStats.currFreeMem += used_block_size + sizeof(Free);// We added the size of the 1 removed free hdr, and used_block_size
	}
}






// --- Private helper methods ---------------

Free* Mem::GetFreeBlock(const uint32_t block_size_required) {

	Free* next_fit = pHeap->pNextFit;

	const Free* const last_addr_we_started_searching_from = next_fit;

	bool found = false;
	while (next_fit != nullptr/* && next_fit != heap_bottom_addr*/) {
		if (next_fit->mBlockSize >= block_size_required) {// this block is available
			found = true;
			break;
		}
		next_fit = next_fit->pFreeNext;
	}

	if (!found) {
		// Now just because next fit is at an end, it doesn't mean there are no free blocks that can accomodate our size.
		// It just means that we'll have to search from the beginning...so set next fit ptr to free head and start search again up until we hit the last search entry point.
		next_fit = pHeap->pFreeHead;
		while (next_fit != last_addr_we_started_searching_from) {
			if (next_fit->mBlockSize >= block_size_required) {// this block is available
				break;
			}
			next_fit = next_fit->pFreeNext;
		}
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
}

void Mem::InsertUsedBlock(Used* used_block_to_insert) {

	// First, let's null out the next/prev ptr's for correctness (make sure we are working with a clean block)
	used_block_to_insert->pUsedNext = nullptr;
	used_block_to_insert->pUsedPrev = nullptr;

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
}

Free* Mem::CoalesceWithAboveAndBelowFreeBlocks(Free* prev_free_block, Free* new_free_block, Free* next_free_block) {

	// So the idea here is to remove next_free_block from our FREE list, and just extend (increase) prev_free_block's size, taking into account the used block (new_free_block) and its header, as well as next_free_block's header
	const bool block_above_prev_free_block_is_free = prev_free_block->mAboveBlockFree;

	Free* const new_large_free_block = prev_free_block;// The address of our new large free block will be here
	// The size of our new_large_free_block has to INCLUDE the size of 1 FREE hdr, 1 USED hdr, but EXCLUDE the size of its OWN FREE hdr
	const uint32_t new_large_free_block_size = prev_free_block->mBlockSize + new_free_block->mBlockSize + next_free_block->mBlockSize + (sizeof(Free) * 2);

	// Check if we'll need to update our next fit ptr to point to new_large_free_block at the end
	bool update_next_fit_ptr = false;
	if (pHeap->pNextFit == nullptr || pHeap->pNextFit == next_free_block) {
		update_next_fit_ptr = true;
	}

	Free* const next_next_free_block = next_free_block->pFreeNext;

	// Now remove next_free_block from our free list
	RemoveFreeBlock(next_free_block);

	new_large_free_block->mBlockSize = new_large_free_block_size;
	new_large_free_block->mAboveBlockFree = block_above_prev_free_block_is_free;

	//const uintptr_t secret_ptr_addr = reinterpret_cast<uintptr_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr);
	placement_new(reinterpret_cast<Free*>(reinterpret_cast<uintptr_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr)), SecretPtr, new_large_free_block);
	//Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Update new_large_free_block's pFreeNext ptr to point to next_next_free_block, and next_next_free_block's pFreePrev ptr to point to new_large_free_block, when applicable
	new_large_free_block->pFreeNext = next_next_free_block;
	if (next_next_free_block != nullptr) {
		next_next_free_block->pFreePrev = new_large_free_block;
	}

	// Update the next fit ptr, if need be
	if (update_next_fit_ptr) {
		pHeap->pNextFit = new_large_free_block;
	}

	return new_large_free_block;
}

Free* Mem::CoalesceWithAboveFreeBlock(Free* prev_free_block, Free* new_free_block) {

	// So the idea here is to just extend (increase) prev_free_block's size, taking into account the used block (new_free_block) and its header
	const bool block_above_prev_free_block_is_free = prev_free_block->mAboveBlockFree;
	
	Free* const new_large_free_block = prev_free_block;// The address of our new large free block will be here
	// The size of our new_large_free_block has to INCLUDE the size of 1 FREE hdr, but EXCLUDE the size of its OWN FREE hdr
	const uint32_t new_large_free_block_size = prev_free_block->mBlockSize + new_free_block->mBlockSize + sizeof(Free);

	// Check if we'll need to update our next fit ptr to point to new_large_free_block at the end
	bool update_next_fit_ptr = false;
	if (pHeap->pNextFit == nullptr) {
		update_next_fit_ptr = true;
	}

	new_large_free_block->mBlockSize = new_large_free_block_size;
	new_large_free_block->mAboveBlockFree = block_above_prev_free_block_is_free;

	//const uintptr_t secret_ptr_addr = reinterpret_cast<uintptr_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr);
	placement_new(reinterpret_cast<Free*>(reinterpret_cast<uintptr_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr)), SecretPtr, new_large_free_block);
	//Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Update the next fit ptr, if need be
	if (update_next_fit_ptr) {
		pHeap->pNextFit = new_large_free_block;
	}

	return new_large_free_block;
}

Free* Mem::CoalesceWithBelowFreeBlock(Free* new_free_block, Free* next_free_block) {

	// The idea here is to remove next_free_block from our FREE list, and just create 1 large free block at new_free_block, and insert it into our FREE list

	Free* new_large_free_block = new_free_block;// The address of our new large free block will be here
	// The size of our new_large_free_block has to INCLUDE the size of 1 FREE hdr, but EXCLUDE the size of its OWN FREE hdr
	const uint32_t new_large_free_block_size = new_free_block->mBlockSize + next_free_block->mBlockSize + sizeof(Free);

	// Check if we'll need to update our next fit ptr to point to new_large_free_block at the end
	bool update_next_fit_ptr = false;
	if (pHeap->pNextFit == nullptr || pHeap->pNextFit == next_free_block) {
		update_next_fit_ptr = true;
	}

	Free* const prev_free_block = next_free_block->pFreePrev;
	Free* const next_next_free_block = next_free_block->pFreeNext;

	// Now remove next_free_block from our free list
	RemoveFreeBlock(next_free_block);

	// Create the new large free block, along with its secret ptr
	new_large_free_block = placement_new(new_large_free_block, Free, new_large_free_block_size);

	//const uintptr_t secret_ptr_addr = reinterpret_cast<uintptr_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr);
	placement_new(reinterpret_cast<Free*>(reinterpret_cast<uintptr_t>(new_large_free_block + 1) + new_large_free_block_size - sizeof(SecretPtr)), SecretPtr, new_large_free_block);
	//Trace::out("secret_ptr address: %p\n", secret_ptr);

	// Now let's insert new_large_free_block in our free list
	new_large_free_block->pFreePrev = prev_free_block;
	if (prev_free_block != nullptr) {
		prev_free_block->pFreeNext = new_large_free_block;
	}
	else {
		pHeap->pFreeHead = new_large_free_block;
	}
	new_large_free_block->pFreeNext = next_next_free_block;
	if (next_next_free_block != nullptr) {
		next_next_free_block->pFreePrev = new_large_free_block;
	}

	// Update the next fit ptr, if need be
	if (update_next_fit_ptr) {
		pHeap->pNextFit = new_large_free_block;
	}

	return new_large_free_block;
}

// ---  End of File ---------------

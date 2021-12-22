//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef MEM_H
#define MEM_H

#include "Heap.h"

class Mem
{
public:
	static const unsigned int HEAP_SIZE = (50 * 1024);

public:
	Mem();	
	Mem(const Mem &) = delete;
	Mem & operator = (const Mem &) = delete;
	~Mem();

	Heap *getHeap();
	void dump();

	// implement these functions
	void free( void * const data );// data points to an address that is at the END of a USED hdr.
	void *malloc( const uint32_t size );// size EXCLUDES the size of what will become a used header, just the raw block size, return end of USED header to new block!
	void initialize( );

private:
	Free* GetFreeBlock(const uint32_t block_size_required);// block_size_required EXCLUDES the free header size
	void RemoveFreeBlock(Free* free_block_to_remove);
	void InsertUsedBlock(Used* used_block_to_insert);// Used blocks are unsorted, and are just pushed in the beginning of the used list
	void InsertFreeBlock(Free* free_block_to_insert);// Free blocks are sorted in ASC memory address
	void RemoveUsedBlock(Used* used_block_to_remove);
	
	Free* CoalesceWithAboveAndBelowFreeBlocks(Free* prev_free_block, Free* new_free_block, Free* next_free_block);
	Free* CoalesceWithAboveFreeBlock(Free* prev_free_block, Free* new_free_block);
	Free* CoalesceWithBelowFreeBlock(Free* new_free_block, Free* next_free_block);

	void UpdateHeapStatisticsAfterPerfectMalloc(const uint32_t malloc_size);
	void UpdateHeapStatisticsAfterPartialMalloc(const uint32_t malloc_size);
	void UpdateHeapStatisticsAfterFree(const uint32_t free_size);

private:
	Heap	*pHeap;
	void	*pRawMem;

};

#endif 

// ---  End of File ---------------

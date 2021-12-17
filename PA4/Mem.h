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
	void free( void * const data );
	void *malloc( const uint32_t size );// size EXCLUDES the size of what will become a used header, just the raw block size.
	void initialize( );


private:
	Heap	*pHeap;
	void	*pRawMem;

};

#endif 

// ---  End of File ---------------

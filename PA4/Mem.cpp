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

void Mem::initialize()
{
	// Add magic here
}

void *Mem::malloc( const uint32_t size )
{
	STUB_PLEASE_REPLACE(size);
	return STUB_PLEASE_REPLACE(0);
}

void Mem::free( void * const data )
{
	STUB_PLEASE_REPLACE(data);	
}


// ---  End of File ---------------

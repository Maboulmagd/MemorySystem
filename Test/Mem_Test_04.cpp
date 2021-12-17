//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"
#include "_UnitTestConfiguration.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h) 
#define UNUSED_VAR(p) ((void *)p)

#if Mem4_Partial_allocate_Test_Enable
TEST( Partial_allocate , MemoryShakeOut, Mem4_Partial_allocate_Test_Enable)
{
	FileIO::Open("Student_", "Test_04");

	fprintf(FileIO::GetHandle(),"Test4: \n\n");

	fprintf(FileIO::GetHandle()," 1) allocate regular size block        \n");

	fprintf(FileIO::GetHandle(),"     Mem mem;                          \n");
	fprintf(FileIO::GetHandle(),"     mem.initialize();                 \n");
   	fprintf(FileIO::GetHandle(),"     void *p = mem.malloc( 200 );      \n");
	fprintf(FileIO::GetHandle(),"     mem.dump();                       \n");

	Mem mem1;
	mem1.initialize();
   	void *p1 = mem1.malloc( 0x200 );
	mem1.dump();

	// Shut up warnings
	UNUSED_VAR(p1);

	FileIO::Close();


	// ----  GENERAL CHECK ------------------------------------------------------

	// ---- Part A: -------------------------------------------------------------
	
	Mem mem;
	mem.initialize();
   	void *p = mem.malloc( 0x200 );

	// ---- Verify A: -----------------------------------------------------------

		Heap *h = mem.getHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x240 );
		CHECK_EQUAL( PTR_FIX(h->pNextFit), 0x240);
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x30);

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 1 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x200);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 1 );
		CHECK_EQUAL( h->mStats.currUsedMem, 0x200 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc5b0 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );


	// ---- Heap Walk ------------------------------------------------------

		// ---- Heap Hdr -------------------------------------------------------

			// HeapHdr Start
			CHECK_EQUAL( PTR_FIX( (uint32_t)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
			// HeapHdr End
			CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap));

		// ----  Block walk ------------------------------------------------------
		
			uint32_t hdrStart;
			uint32_t hdrEnd;
			uint32_t blkStart;
			uint32_t blkEnd; 

		// ---- USED HDR -------------------------------------------

			// Check type
			Used *used = (Used *) h->mStats.heapTopAddr;
			// Should be USED
			CHECK_EQUAL( used->mType, Block::Used );
			// Above Free?
			CHECK_EQUAL(used->mAboveBlockFree, false);
		
			hdrStart = (uint32_t)used;
			hdrEnd   = (uint32_t)used + sizeof(Used);
	
			// Hdr Start
			CHECK_EQUAL( PTR_FIX(hdrStart), 0x30  );
			// Hdr End
			CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
			// Prev
			CHECK_EQUAL( used->pUsedPrev, 0 );
			// Next
			CHECK_EQUAL( used->pUsedNext, 0 );
			// Hdr Size
			CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Used) );
	
		// ---- USED BLOCK ----------------------------------------------------

			blkStart = hdrEnd;
			blkEnd   = blkStart + used->mBlockSize; 
		
			CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
			CHECK_EQUAL( PTR_FIX(blkEnd), 0x240);
			CHECK_EQUAL( PTR_FIX(p), PTR_FIX(hdrEnd));
			CHECK_EQUAL( used->mBlockSize, 0x200 );
	
		// ---- FREE HDR -------------------------------------------

			// Check type
			Free *free = (Free *) blkEnd;
			// Should be USED
			CHECK_EQUAL( free->mType, Block::Free );
			// Above Free
			CHECK_EQUAL(free->mAboveBlockFree, false);
		
			hdrStart = (uint32_t)free;
			hdrEnd   = (uint32_t)free + sizeof(Free);
	
			// Hdr Start
			CHECK_EQUAL( PTR_FIX(hdrStart), 0x240 );
			// Hdr End
			CHECK_EQUAL( PTR_FIX(hdrEnd), 0x250 );
			// Prev
			CHECK_EQUAL( free->pFreePrev, 0 );
			// Next
			CHECK_EQUAL( free->pFreeNext, 0 );
			// Hdr Size
			CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Free) );
	
		// ---- FREE BLOCK ----------------------------------------------------

			blkStart = hdrEnd;
			blkEnd   = blkStart + free->mBlockSize; 
		
			CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
			CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
			CHECK_EQUAL( free->mBlockSize, 0xc5b0 );
}  TEST_END
#endif

// ---  End of File ---------------

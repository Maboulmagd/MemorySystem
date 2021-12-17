//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"
#include "_UnitTestConfiguration.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h)  

#if Mem15_combo_2_Test_Enable
TEST( Mem_combo_2 , MemoryShakeOut, Mem15_combo_2_Test_Enable)
{
	FileIO::Open("Student_", "Test_15");

	fprintf(FileIO::GetHandle(),"Test15: \n\n");

	fprintf(FileIO::GetHandle(),"     Mem mem;                     \n");
	fprintf(FileIO::GetHandle(),"     mem.initialize();            \n");

   	fprintf(FileIO::GetHandle(),"     void *a = mem.malloc( 0x100 );     \n");
   	fprintf(FileIO::GetHandle(),"     void *b = mem.malloc( 0x200 );     \n");
	fprintf(FileIO::GetHandle(),"     void *c = mem.malloc( 0x10 );     \n");
	fprintf(FileIO::GetHandle(),"     void *d = mem.malloc( 0x200 );     \n");

       
	fprintf(FileIO::GetHandle(),"     mem.dump();                        \n");
	fprintf(FileIO::GetHandle(),"     mem.free( d );                     \n");
	fprintf(FileIO::GetHandle(),"     mem.free( b );                     \n");

	fprintf(FileIO::GetHandle(),"     mem.dump();                        \n");

	fprintf(FileIO::GetHandle(),"     void *r = mem.malloc( 0x200 );     \n");
	fprintf(FileIO::GetHandle(),"     mem.dump();                        \n");

	// Part A:
	Mem mem1;
	mem1.initialize();

   	void *a1 = mem1.malloc( 0x100 );
   	void *b1 = mem1.malloc( 0x200 );
	void *c1 = mem1.malloc( 0x10 );
	void *d1 = mem1.malloc( 0x200 );

	mem1.dump();

	// Part B:
	mem1.free( d1 );
	mem1.free( b1 );

	mem1.dump();

	// Part C:
	void *r1 = mem1.malloc( 0x200 );
	mem1.dump();

	// to shut up warnings
	r1 = 0;
	a1 = 0;
	c1 = 0;

	FileIO::Close();

	// ----  GENERAL CHECK ------------------------------------------------------

	// ---- Part A: -------------------------------------------------------------
	
	Mem mem;
	mem.initialize();

   	void *a = mem.malloc( 0x100 );
   	void *b = mem.malloc( 0x200 );
	void *c = mem.malloc( 0x10 );
	void *d = mem.malloc( 0x200 );

	// ---- Verify A: -----------------------------------------------------------

		Heap *h = mem.getHeap();

		CHECK_EQUAL( PTR_FIX(a), 0x40 );
		CHECK_EQUAL( PTR_FIX(b), 0x150 ); 
		CHECK_EQUAL( PTR_FIX(c), 0x360 );
		CHECK_EQUAL( PTR_FIX(d), 0x380 );

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x580 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x370 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 4 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x510);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 4);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x510 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc270 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (uint32_t)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		uint32_t hdrStart;
		uint32_t hdrEnd;
		uint32_t blkStart;
		uint32_t blkEnd; 
		uint32_t secret;
	
	// ---- HDR 1 -------------------------------------------

		// Check type
		Used *used = (Used *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x140 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Used) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x140 );
		CHECK_EQUAL( used->mBlockSize, 0x100 );

	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x140 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x150 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x350 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x30);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x350 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );
		
	// ---- HDR 3 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x350 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x360 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x370 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x140);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x370 );
		CHECK_EQUAL( used->mBlockSize, 0x10 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x370 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x380 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x350);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x580 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );
		
	// ---- Hdr 5 -------------------------------------------

		// Check type
		Free *free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x580 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x590 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0);
	
		// ---- Block ----------------------------------------------------

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xc270 );

		secret = *((uint32_t *)(blkEnd - 4));
		CHECK_EQUAL( PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));
	
	// ---- Part B: -------------------------------------------------------------
	
	mem.free( d );
	mem.free( b );
	
	// ---- Verify B: -----------------------------------------------------------

		h = mem.getHeap();

		CHECK_EQUAL( PTR_FIX(a), 0x40 );
		CHECK_EQUAL( PTR_FIX(b), 0x150 ); 
		CHECK_EQUAL( PTR_FIX(c), 0x360 );
		CHECK_EQUAL( PTR_FIX(d), 0x380 );

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x140 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x350 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 4 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x510);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 2);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x110 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 2 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc680 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (uint32_t)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


	// ---- HDR 1 -------------------------------------------

		// Check type
		used = (Used *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x350 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Used) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x140 );
		CHECK_EQUAL( used->mBlockSize, 0x100 );

	// ---- HDR 2 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be USED
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x140 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x150 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x370);

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x350 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
		
	// ---- HDR 3 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x350 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x360 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x30);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x370 );
		CHECK_EQUAL( used->mBlockSize, 0x10 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x370 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x380 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x140 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800 );
		CHECK_EQUAL( free->mBlockSize, 0xc480 );
	
		secret = *((uint32_t *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

	// ---- Part C: -------------------------------------------------------------
	
	void *r = mem.malloc( 0x200 );
	
	// ---- Verify C: -----------------------------------------------------------


		h = mem.getHeap();

		CHECK_EQUAL( PTR_FIX(r), 0x380 );

		// Sanity check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x140 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x370 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 4 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x510);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 3);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x310 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 2 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc470 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (uint32_t)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


	// ---- HDR 1 -------------------------------------------

		// Check type
		used = (Used *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x350 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Used) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x140 );
		CHECK_EQUAL( used->mBlockSize, 0x100 );

	// ---- HDR 2 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be USED
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x140 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x150 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x580);

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x350 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
		
	// ---- HDR 3 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x350 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x360 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x370 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x30);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x370 );
		CHECK_EQUAL( used->mBlockSize, 0x10 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be free
		CHECK_EQUAL( used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x370 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x380 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x350);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x580 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );
	
	// ---- HDR 5 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x580);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x590);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x0140);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xc270);

		secret = *((uint32_t *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));
}  TEST_END
#endif

// ---  End of File ---------------

//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"
#include "_UnitTestConfiguration.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h) 

#if Mem14_combo_1_Test_Enable
TEST( combo_1 , MemoryShakeOut, Mem14_combo_1_Test_Enable)
{
	FileIO::Open("Student_", "Test_14");

	fprintf(FileIO::GetHandle(),"Test14: \n\n");

	fprintf(FileIO::GetHandle(), "      Mem mem1;                           \n");
	fprintf(FileIO::GetHandle(), "      mem1.initialize();                  \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      void *a1 = mem1.malloc( 0x200 );    \n");
	fprintf(FileIO::GetHandle(), "      void *b1 = mem1.malloc( 0x200 );    \n");
	fprintf(FileIO::GetHandle(), "      void *c1 = mem1.malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "      void *d1 = mem1.malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "      void *e1 = mem1.malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "      void *f1 = mem1.malloc( 0x400 );    \n");
	fprintf(FileIO::GetHandle(), "      void *g1 = mem1.malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      mem1.dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      // Part B:                          \n");
	fprintf(FileIO::GetHandle(), "      mem1.free( a1 );                    \n");
	fprintf(FileIO::GetHandle(), "      mem1.free( c1 );                    \n");
	fprintf(FileIO::GetHandle(), "      mem1.free( f1 );                    \n");
	fprintf(FileIO::GetHandle(), "      mem1.dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      // Part C:                          \n");
	fprintf(FileIO::GetHandle(), "      void *r1 = mem1.malloc( 0x180 );    \n");
	fprintf(FileIO::GetHandle(), "      mem1.dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      void *s1 = mem1.malloc(0xb100);     \n");
	fprintf(FileIO::GetHandle(), "      mem1.dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      void *t1 = mem1.malloc(0x220);      \n");
	fprintf(FileIO::GetHandle(), "      mem1.dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      mem1.free(s1);                      \n");
	fprintf(FileIO::GetHandle(), "      mem1.dump();                        \n");


	// Part A:
	Mem mem1;
	mem1.initialize();

   	void *a1 = mem1.malloc( 0x200 );
   	void *b1 = mem1.malloc( 0x200 );
	void *c1 = mem1.malloc( 0x300 );
	void *d1 = mem1.malloc( 0x300 );
	void *e1 = mem1.malloc( 0x300 );
	void *f1 = mem1.malloc( 0x400 );
	void *g1 = mem1.malloc( 0x300 );

	mem1.dump();

	// Part B:
	mem1.free( a1 );
	mem1.free( c1 );
	mem1.free( f1 );
	mem1.dump();

	// Part C:
	void *r1 = mem1.malloc( 0x180 );
	mem1.dump();

	// Part D:
	void *s1 = mem1.malloc(0xb100);
	mem1.dump();

	// Part E:
	void *t1 = mem1.malloc(0x220);
	mem1.dump();

	// Part F:
	mem1.free(s1);
	mem1.dump();

	// to shut up warnings
	r1 = 0;
	t1 = 0;
	g1 = 0;
	e1 = 0;
	d1 = 0;
	b1 = 0;

	FileIO::Close();

	// ----  GENERAL CHECK ------------------------------------------------------

	// ---- Part A: -------------------------------------------------------------
	
	Mem mem;
	mem.initialize();

   	void *a = mem.malloc( 0x200 );
   	void *b = mem.malloc( 0x200 );
	void *c = mem.malloc( 0x300 );
	void *d = mem.malloc( 0x300 );
	void *e = mem.malloc( 0x300 );
	void *f = mem.malloc( 0x400 );
	void *g = mem.malloc( 0x300 );

	// ---- Verify A: -----------------------------------------------------------

		Heap *h = mem.getHeap();

		CHECK_EQUAL( PTR_FIX(a), 0x40 );
		CHECK_EQUAL( PTR_FIX(b), 0x250 ); 
		CHECK_EQUAL( PTR_FIX(c), 0x460 );
		CHECK_EQUAL( PTR_FIX(d), 0x770 );
		CHECK_EQUAL( PTR_FIX(e), 0xa80 );
		CHECK_EQUAL( PTR_FIX(f), 0xd90 );
		CHECK_EQUAL( PTR_FIX(g), 0x11a0 );

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x14a0 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x1190 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 7 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x1400 );

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 7 );
		CHECK_EQUAL( h->mStats.currUsedMem, 0x1400 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xb350 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (uint32_t)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );

	// ----  Block walk ------------------------------------------------------
		
		uint32_t hdrStart;
		uint32_t hdrEnd;
		uint32_t blkStart;
		uint32_t blkEnd; 
		uint32_t secret;

	// ---- USED HDR 1 -------------------------------------------

		// Check type
		Used *used = (Used *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x240);
		// Next
		CHECK_EQUAL( used->pUsedNext, 0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL( used->mBlockSize, 0x200 );
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x240 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x450 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x30 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Used) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x450);
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
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x450 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x460 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x760 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x240 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x770 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xa70 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x450);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 5 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xa80 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xd80 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 6 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xd90 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x1190 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL( used->mBlockSize, 0x400 );

	// ---- HDR 7 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x11a0 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0xd80);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 8 -------------------------------------------

		// Check type
		Free *free= (Free *) blkEnd;
		// Should be USED
		CHECK_EQUAL( free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x14b0 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xb350 );

		secret = *((uint32_t *)(blkEnd - 4));

		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

	// ---- Part B: -------------------------------------------------------------
	
	mem.free( a );
	mem.free( c );
	mem.free( f );
	
	// ---- Verify B: -----------------------------------------------------------

		h = mem.getHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x1190 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 7 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x1400);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 4);
		CHECK_EQUAL( h->mStats.currUsedMem, 0xb00 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 4 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xbc50 );

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
		free = (Free *) h->mStats.heapTopAddr;
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x450 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Free) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x240 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x240 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x250 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x760 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Used) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x450 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );

	// ---- Hdr 3 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x450 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x460 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x30 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0xd80 );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL( free->mBlockSize, 0x300 );
	
	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x760 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x770 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xa70 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xa70 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );
		
	// ---- HDR 5 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xa70 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xa80 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x1190 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x760 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xd80 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- Hdr 6 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xd80 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xd90 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x450);
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x14a0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL( free->mBlockSize, 0x400 );
	
	// ---- HDR 7 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1190 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x11a0 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0xa70 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x14a0 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- Hdr 8 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x14a0 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x14b0 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xb350 );

		secret = *((uint32_t *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

	// ---- Part C: -------------------------------------------------------------
	
	void *r = mem.malloc( 0x180 );
	
	// ---- Verify C: -----------------------------------------------------------

		h = mem.getHeap();

		CHECK_EQUAL( PTR_FIX(r), 0x14b0);

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x14a0 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 7 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x1400);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 5);
		CHECK_EQUAL( h->mStats.currUsedMem, 0xc80 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 4 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xbac0 );

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
		free = (Free *) h->mStats.heapTopAddr;
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x450 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Free) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x240 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x240 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x250 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x760 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(Used) );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x450 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );

	// ---- Hdr 3 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x450 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x460 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x30 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0xd80);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL( free->mBlockSize, 0x300 );
	
	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x760 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x770 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xa70 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xa70 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );
		
	// ---- HDR 5 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xa70 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xa80 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x1190 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x760 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xd80 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- Hdr 6 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xd80 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x450);
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x1630 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL( free->mBlockSize, 0x400);
	
	// ---- HDR 7 -------------------------------------------

		// Check type
		used = (Used *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mType, Block::Used );
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (uint32_t)used;
		hdrEnd   = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1190 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x11a0 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x14a0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext),0xa70);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x14a0 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 8 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

	// ---- Hdr 9 -------------------------------------------

		// Check type
		free = (Free *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mType, Block::Free );
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (uint32_t)free;
		hdrEnd   = (uint32_t)free + sizeof(Free);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1630 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x1640 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xb1c0 );

		// ---- Part D: -------------------------------------------------------------

		void *s = mem.malloc(0xb100);

		// ---- Verify D: -----------------------------------------------------------

		h = mem.getHeap();

		CHECK_EQUAL(PTR_FIX(s), 0x1640);

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x30);
		CHECK_EQUAL(PTR_FIX(h->pUsedHead), 0x1630);

		// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 7);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0xbd80);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 6);
		CHECK_EQUAL(h->mStats.currUsedMem, 0xbd80);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 4);
		CHECK_EQUAL(h->mStats.currFreeMem, 0x9b0);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);

		// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL(PTR_FIX((uint32_t)h->mStats.heapTopAddr - sizeof(Heap)), 0);
		// HeapHdr End
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		// ---- HDR 1 -------------------------------------------

		// Check type
		free = (Free *)h->mStats.heapTopAddr;
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x30);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x40);
		// Prev
		CHECK_EQUAL(free->pFreePrev, 0x0);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x450);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Free));

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL(free->mBlockSize, 0x200);

		// ---- HDR 2 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x240);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x760);
		// Next
		CHECK_EQUAL(used->pUsedNext, 0x0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Used));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x450);
		CHECK_EQUAL(used->mBlockSize, 0x200);

		// ---- Hdr 3 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x450);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x460);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x30);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xd80);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL(free->mBlockSize, 0x300);

		// ---- HDR 4 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x770);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0xa70);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 5 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xa80);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1190);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- Hdr 6 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x450);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xc740);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL(free->mBlockSize, 0x400);

		// ---- HDR 7 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x11a0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x14a0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 8 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1630);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

		// ---- HDR 9 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1630);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x1640);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x14a0);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc740);
		CHECK_EQUAL(used->mBlockSize, 0xb100);

		// ---- Hdr 10 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xc740);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xc750);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xb0);

		// ---- Part E: -------------------------------------------------------------

		void *t = mem.malloc(0x220);

		// ---- Verify E: -----------------------------------------------------------

		h = mem.getHeap();

		CHECK_EQUAL(PTR_FIX(t), 0x460);

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x30);
		CHECK_EQUAL(PTR_FIX(h->pUsedHead), 0x450);

		// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 7);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0xbfa0);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 7);
		CHECK_EQUAL(h->mStats.currUsedMem, 0xbfa0);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 4);
		CHECK_EQUAL(h->mStats.currFreeMem, 0x780);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);

		// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL(PTR_FIX((uint32_t)h->mStats.heapTopAddr - sizeof(Heap)), 0);
		// HeapHdr End
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		// ---- HDR 1 -------------------------------------------

		// Check type
		free = (Free *)h->mStats.heapTopAddr;
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x30);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x40);
		// Prev
		CHECK_EQUAL(free->pFreePrev, 0x0);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x680);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Free));

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL(free->mBlockSize, 0x200);

		// ---- HDR 2 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x240);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x760);
		// Next
		CHECK_EQUAL(used->pUsedNext, 0x0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Used));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x450);
		CHECK_EQUAL(used->mBlockSize, 0x200);

		// ---- HDR 3 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x450);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x460);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1630);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Used));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x680);
		CHECK_EQUAL(used->mBlockSize, 0x220);

		// ---- Hdr 4 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x680);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x690);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x30);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xd80);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL(free->mBlockSize, 0xd0);

		// ---- HDR 5 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x770);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0xa70);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 6 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xa80);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1190);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- Hdr 7 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x680);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xc740);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL(free->mBlockSize, 0x400);

		// ---- HDR 8 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x11a0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x14a0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 9 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1630);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

		// ---- HDR 10 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1630);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x1640);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x450);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x14a0);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc740);
		CHECK_EQUAL(used->mBlockSize, 0xb100);

		// ---- Hdr 11 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xc740);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xc750);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xb0);

		// ---- Part F: -------------------------------------------------------------

		mem.free(s);

		// ---- Verify F: -----------------------------------------------------------

		h = mem.getHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x30);
		CHECK_EQUAL(PTR_FIX(h->pUsedHead), 0x450);

		// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 7);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0xbfa0);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 6);
		CHECK_EQUAL(h->mStats.currUsedMem, 0xea0);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 4);
		CHECK_EQUAL(h->mStats.currFreeMem, 0xb890);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);

		// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL(PTR_FIX((uint32_t)h->mStats.heapTopAddr - sizeof(Heap)), 0);
		// HeapHdr End
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		// ---- HDR 1 -------------------------------------------

		// Check type
		free = (Free *)h->mStats.heapTopAddr;
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x30);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x40);
		// Prev
		CHECK_EQUAL(free->pFreePrev, 0x0);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x680);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Free));

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL(free->mBlockSize, 0x200);

		// ---- HDR 2 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x240);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x760);
		// Next
		CHECK_EQUAL(used->pUsedNext, 0x0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Used));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x450);
		CHECK_EQUAL(used->mBlockSize, 0x200);

		// ---- HDR 3 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x450);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x460);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x14a0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Used));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x680);
		CHECK_EQUAL(used->mBlockSize, 0x220);

		// ---- Hdr 4 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x680);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x690);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x30);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xd80);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL(free->mBlockSize, 0xd0);

		// ---- HDR 5 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x770);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0xa70);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 6 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xa80);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1190);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- Hdr 7 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x680);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x1630);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL(free->mBlockSize, 0x400);

		// ---- HDR 8 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x11a0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x14a0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 9 -------------------------------------------

		// Check type
		used = (Used *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mType, Block::Used);
		// Above Free
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (uint32_t)used;
		hdrEnd = (uint32_t)used + sizeof(Used);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x450);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

		// ---- Hdr 10 -------------------------------------------

		// Check type
		free = (Free *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mType, Block::Free);
		// Above Free
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (uint32_t)free;
		hdrEnd = (uint32_t)free + sizeof(Free);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1630);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x1640);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xb1c0);

}  TEST_END
#endif

// ---  End of File ---------------

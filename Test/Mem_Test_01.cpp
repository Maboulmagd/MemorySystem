//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"
#include "_UnitTestConfiguration.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0 : ((unsigned int)x - (unsigned int)h) 

#if Mem1_Initialize_Test_Enable
TEST(initialize, MemoryShakeOut, Mem1_Initialize_Test_Enable)
{
	FileIO::Open("Student_", "Test_01");

	fprintf(FileIO::GetHandle(), "Test 1: \n\n");

	fprintf(FileIO::GetHandle(), "     Mem mem;               \n");
	fprintf(FileIO::GetHandle(), "     mem.initialize();      \n");
	fprintf(FileIO::GetHandle(), "     mem.dump();            \n");

	Mem mem;
	mem.initialize();
	mem.dump();

	FileIO::Close();

	// ----  GENERAL CHECK ------------------------------------------------------

		Heap *h = mem.getHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// Used / Free
		CHECK_EQUAL(h->pUsedHead, 0);
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x30);
		CHECK_EQUAL(PTR_FIX(h->pNextFit), 0x30);

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 0);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 0);
		CHECK_EQUAL(h->mStats.currUsedMem, 0);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 1);
		CHECK_EQUAL(h->mStats.currFreeMem, 0xc7C0);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);


	// ---- Heap Walk ------------------------------------------------------

		// ---- Heap Hdr -------------------------------------------------------

			// HeapHdr Start
			CHECK_EQUAL(PTR_FIX((uint32_t)h->mStats.heapTopAddr - sizeof(Heap)), 0);
			// HeapHdr End
			CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap));


		// ----  Block walk ------------------------------------------------------

			uint32_t hdrStart;
			uint32_t hdrEnd;
			uint32_t blkStart;
			uint32_t blkEnd;

		// ---- HDR -------------------------------------------

			// Check type
			Free *free = (Free *)h->mStats.heapTopAddr;
			// Should be FREE
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
			CHECK_EQUAL(free->pFreePrev, 0);
			// Next
			CHECK_EQUAL(free->pFreeNext, 0);

			// Hdr Size
			CHECK_EQUAL(hdrEnd - hdrStart, sizeof(Free));

		// ---- BLOCK ----------------------------------------------------

			blkStart = hdrEnd;
			blkEnd = blkStart + free->mBlockSize;

			CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
			CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
			CHECK_EQUAL(free->mBlockSize, 0xc7c0);

} TEST_END
#endif

// ---  End of File ---------------

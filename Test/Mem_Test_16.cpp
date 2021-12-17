//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2019
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"
#include "_UnitTestConfiguration.h"

#include "Mem.h"

#include "Trace.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h)  
#define UNUSED_VAR(v) ((void *)v)

#if Mem16_Secret_pointers_Test_Enable
TEST( Secret_pointers , MemoryShakeOut, Mem16_Secret_pointers_Test_Enable)
{
	FileIO::Open("Student_", "Test_16");

	fprintf(FileIO::GetHandle(),"Test16: \n\n");

	// Part A:
	fprintf(FileIO::GetHandle(),"      Mem mem;\n");
	fprintf(FileIO::GetHandle(),"      mem.initialize();\n");

	fprintf(FileIO::GetHandle(),"      void *a1 = mem.malloc(0x100);\n");
	fprintf(FileIO::GetHandle(),"      void *a2 = mem.malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *a3 = mem.malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *a4 = mem.malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *a5 = mem.malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *b1 = mem.malloc(0x200);\n");
	fprintf(FileIO::GetHandle(),"      void *c1 = mem.malloc(0x300);\n");
	fprintf(FileIO::GetHandle(),"      void *c2 = mem.malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c3 = mem.malloc(0x30);\n");

	fprintf(FileIO::GetHandle(),"      void *c4 = mem.malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c5 = mem.malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c6 = mem.malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c7 = mem.malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *d1 = mem.malloc(0x200);\n");

	fprintf(FileIO::GetHandle(),"      void *d2 = mem.malloc(0x40);\n");
	fprintf(FileIO::GetHandle(),"      void *d3 = mem.malloc(0x60);\n");
	fprintf(FileIO::GetHandle(),"      void *e1 = mem.malloc(0x100);\n");
	fprintf(FileIO::GetHandle(),"      void *f1 = mem.malloc(0x500);\n");
	fprintf(FileIO::GetHandle(),"      void *g1 = mem.malloc(0x100);\n\n");

	fprintf(FileIO::GetHandle(),"      mem.dump();\n\n");

	fprintf(FileIO::GetHandle(),"      mem.free(d1);\n");
	fprintf(FileIO::GetHandle(),"      mem.free(b1);\n");
	fprintf(FileIO::GetHandle(),"      mem.free(f1);\n");
	fprintf(FileIO::GetHandle(),"      mem.free(a1);\n");
	fprintf(FileIO::GetHandle(),"      mem.free(a4);\n");
	fprintf(FileIO::GetHandle(),"      mem.free(c7);\n");
	fprintf(FileIO::GetHandle(),"      mem.free(c5);\n");
	fprintf(FileIO::GetHandle(),"      mem.free(c2);\n\n");

	fprintf(FileIO::GetHandle(),"      mem.dump();\n\n");

	fprintf(FileIO::GetHandle(),"      mem1.free(d2);\n");
	fprintf(FileIO::GetHandle(),"      mem1.free(c4);\n");
	fprintf(FileIO::GetHandle(),"      mem1.free(g1);\n\n");

	fprintf(FileIO::GetHandle(),"      mem1.dump();\n\n");


	//---------------------------------------------------------
	// Part A:
	//---------------------------------------------------------
	Mem mem1;
	mem1.initialize();

   	void *a1 = mem1.malloc( 0x100 );
	void *a2 = mem1.malloc(0x20);
	void *a3 = mem1.malloc(0x20);
	void *a4 = mem1.malloc(0x20);
	void *a5 = mem1.malloc(0x20);
   	void *b1 = mem1.malloc( 0x200 );
	void *c1 = mem1.malloc( 0x300 );
	void *c2 = mem1.malloc(0x30);
	void *c3 = mem1.malloc(0x30);

	void *c4 = mem1.malloc(0x30);
	void *c5 = mem1.malloc(0x30);
	void *c6 = mem1.malloc(0x30);
	void *c7 = mem1.malloc(0x30);
	void *d1 = mem1.malloc( 0x200 );

	void *d2 = mem1.malloc(0x40);
	void *d3 = mem1.malloc(0x60);
	void *e1 = mem1.malloc( 0x100);
	void *f1 = mem1.malloc(0x500);
	void *g1 = mem1.malloc(0x100);

	UNUSED_VAR(c1);
	UNUSED_VAR(e1);
	UNUSED_VAR(g1);
	UNUSED_VAR(a1);
	UNUSED_VAR(a2);
	UNUSED_VAR(a3);
	UNUSED_VAR(a5);
	UNUSED_VAR(c3);
	UNUSED_VAR(c6);
	UNUSED_VAR(c4);
	UNUSED_VAR(d2);	
	UNUSED_VAR(d3);

	// ---- Verify A: -----------------------------------------------------------

	Heap *h = mem1.getHeap();

	CHECK_EQUAL(PTR_FIX(a1), 0x40); 
	CHECK_EQUAL(PTR_FIX(a2), 0x150); 
	CHECK_EQUAL(PTR_FIX(a3), 0x180); 
	CHECK_EQUAL(PTR_FIX(a4), 0x1b0); 
	CHECK_EQUAL(PTR_FIX(a5), 0x1e0); 
	CHECK_EQUAL(PTR_FIX(b1), 0x210); 
	CHECK_EQUAL(PTR_FIX(c1), 0x420); 
	CHECK_EQUAL(PTR_FIX(c2), 0x730); 
	CHECK_EQUAL(PTR_FIX(c3), 0x770); 

	CHECK_EQUAL(PTR_FIX(c4), 0x7b0); 
	CHECK_EQUAL(PTR_FIX(c5), 0x7f0);
	CHECK_EQUAL(PTR_FIX(c6), 0x830);
	CHECK_EQUAL(PTR_FIX(c7), 0x870); 
	CHECK_EQUAL(PTR_FIX(d1), 0x8b0);

	CHECK_EQUAL(PTR_FIX(d2), 0xac0); 
	CHECK_EQUAL(PTR_FIX(d3), 0xb10); 
	CHECK_EQUAL(PTR_FIX(e1), 0xb80); 
	CHECK_EQUAL(PTR_FIX(f1), 0xc90);
	CHECK_EQUAL(PTR_FIX(g1), 0x11a0); 

	mem1.dump();

	//---------------------------------------------------------
	// Part B:
	//---------------------------------------------------------

	mem1.free( d1 );
	mem1.free( b1 );
	mem1.free(f1);
	mem1.free(a1);
	mem1.free(a4);
	mem1.free(c7);
	mem1.free(c5);
	mem1.free(c2);

	Used *used;
	uint32_t  *secret;
	Free *free;

	// Check for Secret Pointers, given pointer
	used = (Used *)a2 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x140);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free =  (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x30);

	// Check for Secret Pointers, given pointer
	used = (Used *)a5 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x1d0);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x1a0);

	// Check for Secret Pointers, given pointer
	used = (Used *)c1 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x410);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x200);

	// Check for Secret Pointers, given pointer
	used = (Used *)c3 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x760);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x720);

	// Check for Secret Pointers, given pointer
	used = (Used *)c6 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x820);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x7e0);

	// Check for Secret Pointers, given pointer
	used = (Used *)d2 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0xab0);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x860);

	// Check for Secret Pointers, given pointer
	used = (Used *)g1 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x1190);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0xc80);

	mem1.dump();

	//---------------------------------------------------------
	// Part C:
	//---------------------------------------------------------
	//
	//  READ the next 3 Frees... here is the real test
	//  If it doesn't pass inspection - you lose points 
	//
	//---------------------------------------------------------

	// This should not use While loop on the Free list
	// I will walk into you function and verify - Keenan
	mem1.free(d2);

	// This should not use While loop on the Free list
	// I will walk into you function and verify - Keenan
	mem1.free(c4);

	// This should not use While loop on the Free list
	// I will walk into you function and verify - Keenan
	mem1.free(g1);

	// Check for Secret Pointers, given pointer
	used = (Used *)a2 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x140);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x30);

	// Check for Secret Pointers, given pointer
	used = (Used *)a5 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x1d0);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x1a0);

	// Check for Secret Pointers, given pointer
	used = (Used *)c1 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x410);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x200);

	// Check for Secret Pointers, given pointer
	used = (Used *)c3 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x760);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x720);

	// Check for Secret Pointers, given pointer
	used = (Used *)c6 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x820);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x7a0);

	// Check for Secret Pointers, given pointer
	used = (Used *)d3 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0xb00);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (uint32_t *)((uint32_t)used - 4);
	free = (Free *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x860);

	mem1.dump();
	FileIO::Close();

}  TEST_END
#endif

// ---  End of File ---------------

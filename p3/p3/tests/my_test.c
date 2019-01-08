/*my_test */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

int main() {
	int v = Mem_Init(4096);
	// make the second pointer too large to fit in memory
	assert(v == 0);

	// Mem_Alloc() tests: 
	int* ptr[8];	
	ptr[0] = (int*) Mem_Alloc(1); // test allocating one byte where rounding up by 4 is necessary
	Mem_Dump();
 	ptr[1] = (int*) Mem_Alloc(16); // test allocating 16 bytes
	Mem_Dump();
	ptr[2] = (int*) Mem_Alloc(0); // test allocating 0 bytes
	Mem_Dump();
	ptr[3] = (int*) Mem_Alloc(-1); // test allocating negative memory
	Mem_Dump();
	ptr[4] = (int*) Mem_Alloc(256); 
	Mem_Dump();
	ptr[5] = (int*) Mem_Alloc(2048);
	Mem_Dump();
	ptr[6] = (int*) Mem_Alloc(4200); // test allocating memory too large to fit in heap
	Mem_Dump();
	ptr[7] = (int*) Mem_Alloc(233); // test allocating one byte
	Mem_Dump();

	while(Mem_Alloc(300) != NULL); // fill up the rest of the heap
	Mem_Dump();

	// Mem_Free()/coalescing tests:
	Mem_Free(NULL); // free null
	Mem_Free(ptr[2]); // free ptr that never shouldn't have allocated memory
	Mem_Dump();
	Mem_Free(ptr[5]); // free memory so that memory can be inserted
	Mem_Dump();
	Mem_Free(ptr[4]); // free adjacent block of memory
	Mem_Dump();
	ptr[2] = (int*) Mem_Alloc(1997);
	Mem_Dump();
	
 	
	Mem_Dump();
	exit(0);
}

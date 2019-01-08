/*a few allocations in multiples of 4 bytes*/
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);		Mem_Dump();
   assert(Mem_Alloc(4) != NULL);	Mem_Dump();
   assert(Mem_Alloc(8) != NULL);	Mem_Dump();
   assert(Mem_Alloc(16) != NULL);	Mem_Dump();
   assert(Mem_Alloc(4) != NULL);	Mem_Dump();
   exit(0);
}

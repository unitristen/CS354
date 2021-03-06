/* many odd sized allocations */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);		Mem_Dump();
   assert(Mem_Alloc(1) != NULL);	Mem_Dump();
   assert(Mem_Alloc(5) != NULL);	Mem_Dump();
   assert(Mem_Alloc(14) != NULL);	Mem_Dump();
   assert(Mem_Alloc(8) != NULL);	Mem_Dump();
   assert(Mem_Alloc(1) != NULL);	Mem_Dump();
   assert(Mem_Alloc(4) != NULL);	Mem_Dump();
   assert(Mem_Alloc(9) != NULL);	Mem_Dump();
   assert(Mem_Alloc(33) != NULL);	Mem_Dump();
   assert(Mem_Alloc(55) != NULL);	Mem_Dump();

   exit(0);
}

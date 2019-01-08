/* second allocation is too big to fit */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);		Mem_Dump();
   assert(Mem_Alloc(2048) != NULL);	Mem_Dump();
   assert(Mem_Alloc(2047) == NULL);	Mem_Dump();

   exit(0);
}

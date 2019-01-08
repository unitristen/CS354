#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

int main() {
int v = Mem_Init(4);
assert(v == 0);
int* ptr[5];
ptr[0] = ptr[1] = ptr[2] = ptr[3] = ptr[4] = 0;
  
ptr[0] = (int*) Mem_Alloc(1);
Mem_Dump();
ptr[1] = (int*) Mem_Alloc(5);
Mem_Dump();
ptr[2] = (int*) Mem_Alloc(9);
Mem_Dump();
ptr[3] = (int*) Mem_Alloc(13);
Mem_Dump();
ptr[4] = (int*) Mem_Alloc(4036);
Mem_Dump();

exit(0);
}

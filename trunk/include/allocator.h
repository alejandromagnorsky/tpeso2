#include "../include/defs.h"

#define MEMORY_BASE 0				// Memory start.
#define KERNEL_LIMIT 0x400000		// Kernel size.
#define MEMORY_LIMIT 0x2000000		// Total memory size (32M).
#define PAGE_SIZE 0x1000			// Page size (4K).
#define PAGE_QTY ((MEMORY_LIMIT - MEMORY_BASE) / PAGE_SIZE)	// Size of usable memory in pages.


int binaryPow(int exp);
void * allocator();
void deallocator(void * ptr);

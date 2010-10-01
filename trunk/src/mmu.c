#include "../include/mmu.h"
#include "../include/defs.h"


/* ****************************************************************************************************** */
/* ****************************************************************************************************** */
/* TOdO ESTO PASARLO DESPUÉS A MMU.H */

#define MEMORY_BASE 0						// Memory start.		
#define MEMORY_LIMIT (32*1024*1024)			// Total memory size (32M).
#define PAGE_SIZE 4096						// Page size (4K).
#define PAGE_QTY (MEMORY_LIMIT / PAGE_SIZE)	// Size of memory in pages.

typedef enum { NOT_PRESENT, PRESENT } Presence;

char page_map [PAGE_QTY/8] = { 0 };	// Page frames information.
unsigned short byte_ptr = 0;		// Pointer to the first available set of pages with at
									// least one available page in memory (A BYTE in BITMAP).
char bit_ptr = 0;					// Index of the first empty bit in a byte (A BIT IN A 'byte_ptr').

int binaryPow(int exp);
void * allocator();
void deallocator(void * ptr);

/* Fin includes temporal. */
/* ****************************************************************************************************** */
/* ****************************************************************************************************** */

/* 'binaryPow':
  		Returns 2 to the power of 'exp'.
*/
int binaryPow(int exp){
	int ans = 2;
	if (exp == 0) return 1;
	while(exp-- > 1) ans *= ans;
	return ans;
}

/* 'allocator':
  		Returns a pointer to a free page.
  		NULL in case there is no free page in memory.
*/
void * allocator(){
	int page_number = (byte_ptr * 8) + bit_ptr;
	int not_present = binaryPow(bit_ptr);
	
	if (page_number == PAGE_QTY) return NULL;	// !!!!! MEMORY IS FULL !!!!!
	else page_map[byte_ptr] |= not_present;		// Updates page_map
	
	/* Updates pointer */
	if (++bit_ptr == 8){ bit_ptr = 0; byte_ptr++; }
	
	return (void *)(page_number * PAGE_SIZE);
}

/* 'deallocator':
  		Given a pointer 'ptr', frees its associated page in memory.
*/ 
void deallocator(void * ptr){
	unsigned short page_byte = ((unsigned int) ptr) / (PAGE_SIZE*8);
	char page_bit = (((unsigned int) ptr) / PAGE_SIZE) % 8;
	char present = ~binaryPow(page_bit);
	page_map[page_byte] &= present;
}



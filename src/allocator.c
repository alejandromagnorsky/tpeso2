#include "../include/allocator.h"

char page_map [PAGE_QTY/8];	// Page frames information.
unsigned short byte_ptr = 0;		// Pointer to the first available set of pages with at
									// least one available page in memory (A BYTE in BITMAP).
char bit_ptr = 0;					// Index of the first empty bit in a byte (A BIT IN A 'byte_ptr').


/* 'binaryPow':
  		Returns 2 to the power of 'exp'.
*/
int binaryPow(int exp){
	int ans = 2;
	if (exp == 0) return 1;
	while(exp-- > 1) ans *= 2;
	return ans;
}

/* 'allocator':
  		Returns a pointer to a free page.
  		NULL in case there is no free page in memory.
*/
void * allocator(){
	int page_number = (byte_ptr * 8) + bit_ptr;
	if (page_number == PAGE_QTY){ printf("FULL\n"); return NULL; }	// !!!!! MEMORY IS FULL !!!!!

	page_map[byte_ptr] |= binaryPow(bit_ptr);		// Updates page_map

	/* Updates pointer */
	if (++bit_ptr == 8){ bit_ptr = 0; byte_ptr++; }
	
	return (void *)(page_number * PAGE_SIZE);
}


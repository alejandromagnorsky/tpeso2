//#include "../include/mmu.h"


/* ****************************************************************************************************** */
/* ****************************************************************************************************** */
// INCLUDES TEMPORAL
typedef enum { NOT_PRESENT, PRESENT } Presence;

typedef struct {
	unsigned int base_addr;
	Presence p;
} Page_descriptor;

Page_descriptor pages [PAGE_QUANT];

void __paging();

/* Fin includes temporal. */
/* ****************************************************************************************************** */
/* ****************************************************************************************************** */

/* __PAGING:
	- enables paging unit.
	- makes CR3 point to the page directory.
*/
void __paging(){
	unsigned int i, cr0;

	/* Enables paging by setting 31th bit of CR0 in 1. */
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;				// bitwise-OR CR0 with 1000 0000 ... 0000 -> Enables paging bit.
	asm volatile("mov %0, %%cr0":: "b"(cr0));	// Load CR0 register with the new value.

	/* CR3 now points to the begginning of upper memory. */
	asm volatile("mov %0, %%cr3":: "b"(base));
}

#include "../include/mmu.h"


/* ****************************************************************************************************** */
/* ****************************************************************************************************** */
/* TOdO ESTO PASARLO DESPUÉS A MMU.H */

/* Ver bien qué onda esta sttuct Page_descriptor. Así como está es solo de prueba. */
// CONSTANTES HIPER TEMPORALES; después ver bien como tratar esto.
#define NULL 0	// !!!!!!!
#define UPPER_START 1048576	// Comienzo de la upper memory (1 MiB) en bytes.
#define UPPER_SIZE 31680	// CONSTANTE RE-TEMPORAL con el tamaño de la upper memory EN KiB.
#define PAGE_SIZE 4096		//Tamaño de las páginas en bytes, 4 KiB. no creo que las hagamos variables.
#define PAGE_QUANT (int)(UPPER_SIZE/PAGE_SIZE)

typedef enum { NOT_PRESENT, PRESENT } Presence;

typedef struct {
	unsigned int base_addr;
	Presence p;
} Page_descriptor;

Page_descriptor pages [PAGE_QUANT];

void __paging();
void * malloc(unsigned int size);
int free();

/* Fin includes temporal. */
/* ****************************************************************************************************** */
/* ****************************************************************************************************** */

/* __PAGING:
	- enables paging unit.
	- makes CR3 point to the page directory.
	- fills page directory with page tables and page tables with PRESENT pages.
*/
void __paging(){
	unsigned int i, cr0, base = UPPER_START;

	/* Enables paging by setting 31th bit of CR0 in 1. */
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;				// bitwise-OR CR0 with 1000 0000 ... 0000 -> Enables paging bit.
	asm volatile("mov %0, %%cr0":: "b"(cr0));	// Load CR0 register with the new value.

	/* CR3 now points to the begginning of upper memory. */
	asm volatile("mov %0, %%cr3":: "b"(base));

	// THIS (!!??)
	for (i=0; i<PAGE_QUANT; i++, base+=PAGE_SIZE){
		pages[i].base_addr = base;
		pages[i].p = PRESENT;
	}
}

/* MALLOC:
  	returns a pointer to the first free memory chunk of size n*PAGE_SIZE, being n the minimun integer
	satisfying n*PAGE_SIZE >= size. NULL in case there is not such available contiguous space.
*/
void * malloc(unsigned int size){
	int i, n = 0;
	for(i=0; i<PAGE_QUANT; i++){
		if (pages[i].p == PRESENT){
			n++;
			if (n*PAGE_SIZE >= size){
				pages[i-n+1].p = NOT_PRESENT;
				return (void *)pages[i-n+1].base_addr;
			}
		}
	}
	return NULL;
}

/* FREE:
  	given a pointer ptr, it sets its corresponding page presence bit in PRESENT.
*/
int free(void * ptr){
	int pg = (unsigned int) ptr / PAGE_SIZE;
	if (pg < 0 || pg >= PAGE_QUANT){
		return -1;
	} else {
		pages[pg].p = PRESENT;
		return 0;
	}
}



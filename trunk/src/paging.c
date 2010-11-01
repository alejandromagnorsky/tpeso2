#include "../include/allocator.h"
#include "../include/kasm.h"


/* ******************************************************************************************************
   ******************************************************************************************************
	COSAS PARA TERMINAR DE PAGING.C:
		1)  Cuando esto esté mas avanzoado, hacer que se cargue el tamaño de la memoria dinamicamente
			y se pagine en función de eso.
		3) Terminar la función MAP().
		

   ******************************************************************************************************
   ****************************************************************************************************** */

/* ****************************************************************************************************** 
   ******************************************************************************************************
	PHYSICAL MEMORY MAP
	 ___________________
	|					| <---- 0x0: Beginning of memory
	|					|
	|					|
	|	K E R N E L		|
	|					|
	|					|
	|- - - - - - - - - -| <---- 0x3FF000: Beginning of page directory page (still in kernel's memory).
	|	Page directory	|
	===================== <---- 0x400000: End of kernel memory
	|	Page table 0	|
	---------------------
	|	Page table 1	|
	---------------------
	|	Page table 2	|
	---------------------
	|	Page table 3	|
	---------------------
	|	Page table 4	|
	---------------------
	|	Page table 5	|
	---------------------
	|	Page table 6	|
	---------------------
	|	Page table 7	|
	===================== <--------\     <---- 0x408000: Beginning of available memory.
	|					|			|
-------------------------------		|
-------------------------------		|---> "Usable" memory.
	|					|			|
	|___________________| <--------/     <---- End of memory.
	
   ******************************************************************************************************
   ****************************************************************************************************** */


/* ****************************************************************************************************** */
/* ****************************************************************************************************** */

// INCLUDES TEMPORAL
#define PAGE_DIR_QTY (MEMORY_LIMIT/(PAGE_SIZE*1024))
#define EMPTY_ADDRESS 0
#define P_FLAG 1
#define RW_FLAG 2
#define U_FLAG 4

void paging();
void * map(void * phys_addr);

/* Fin includes temporal. */
/* ****************************************************************************************************** */
/* ****************************************************************************************************** */

/* PAGING:
	- enables paging unit.
*/
void paging(){
	unsigned int i, cr0;
	unsigned int * page_dir = (unsigned int *)((KERNEL_LIMIT - PAGE_SIZE) & 0xFFFFF000);	// page_dir is the last page in kernel's memory chunk.
	unsigned int * page_table = (unsigned int *)((unsigned int)page_dir + PAGE_SIZE);
	
	/*
	 * 	As we are working on a (virtual) 32M-memory hardware, we
	 * 	can use only 8 page directory entries to map all the memory.
	 */
	 
	/* FILLS ENTRIES IN PAGE DIRECTORY */	
	for(i=0; i < PAGE_SIZE/4; i++)
		page_dir[i] = ((unsigned int)page_table + i * PAGE_SIZE) | U_FLAG | RW_FLAG | P_FLAG;
		
	/* FILLS ENTRIES IN EVERY PAGE TABLE */
	/* 		- 1st page table maps the whole kernel's 4M memory chunk.
	 * 		- Last entry in 1st page table maps page directory (page_table[1023]).
	 *		- From 2nd page table to 8th page table:
	 * 			Let PDE-X-Y be the Yth entry in the Xth page directory entry.
	 * 			So, from now on, PDE-X-Y (X > 0) will map physical address of PDE-Y.
	 * 				E.g.: PDE-1-0 entry will map the physical address of whole PDE-0.
	 */
	 
	for(i=0; i < (PAGE_DIR_QTY * 1024) ; i++)	// 8 page directories * 1024 entries each one
		page_table[i] = (unsigned int)allocator() | U_FLAG | RW_FLAG | P_FLAG;
	
/*	// TESTING 0x500000 ptr WORKS!!!!!!!!
	unsigned int * ptr1 = (unsigned int *)((0x400000 + 4096 + 4*256));
	unsigned int aux = *ptr1;
	*ptr1 = aux & 0xFFFFFFF0;
*/

	/* CR3 now points to the page directory. */
	_write_cr3( (unsigned int)page_dir );
	
	/* Enables paging by setting 31th bit of CR0 in 1. */
	_write_cr0( _read_cr0() | 0x80000000 );	// bitwise-OR CR0 with 1000 0000 ... 0000 -> Enables paging bit.
	
}

/*	******************************************************************************************************
	***************************************** ( 3 ) ******************************************************
	******************************************************************************************************
*/
void * map(void * phys_addr){
	return NULL;
/* // NO BORRO ESTO POR LAS DUDAS
	int byte_ptr, bit_ptr;
	for(byte_ptr = 0; byte_ptr < (PAGE_QTY/32) && pages[byte_ptr] == 0xFFFFFFFF; byte_ptr++);
	
	if(byte_ptr == (PAGE_QTY/32)) return NULL;	// No page available.
	
	for(bit_ptr = 1; (~pages[byte_ptr] & bit_ptr) == 0; byte_ptr *= 2);
	
	unsigned int phys_base = (unsigned int)phys_addr & 0xFFFFF000;
	int phys_offset = (unsigned int)phys_addr & 0xFFF;
	
	unsigned int * virtual_addr = 0;
	int page_table = ((byte_ptr*32) + bit_ptr) / 1024;
	int page_frame = ((byte_ptr*32) + bit_ptr) % 1024;
*/
	
}


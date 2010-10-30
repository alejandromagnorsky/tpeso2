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
-----------------------------		|
-----------------------------		|---> "Usable" memory.
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
#define MAX_PROCESSES 16

void paging();
void * map(void * phys_addr);

char __page_status[PAGE_QTY/8];
char __active_pages[PAGE_QTY/8];

typedef struct {
	int pid;		// Process id
	void * d_page;	// Data page
	void * s_page;	// Stack page
} __Process_pages;

__Process_pages __pages_per_process[MAX_PROCESSES];

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
	 
	for(i=0; i < (PAGE_DIR_QTY * 1024) ; i++)	// 8 page directory entries * 1024 entries each one
		page_table[i] = (unsigned int)allocator() | U_FLAG | RW_FLAG | P_FLAG;
	
	/* Initializes __page_status and __active_pages */
	for (i=0; i < (PAGE_QTY/8); i++){
		__page_status[i] = 0x00;
		__active_pages[i] = 0xFF;
	}
	
	/* CR3 now points to the page directory. */
	_write_cr3( (unsigned int)page_dir );
	
	/* Enables paging by setting 31th bit of CR0 in 1. */
	_write_cr0( _read_cr0() | 0x80000000 );	// bitwise-OR CR0 with 1000 0000 ... 0000 -> Enables paging bit.
	

}

/*	******************************************************************************************************
	***************************************** ( 3 ) ******************************************************
	******************************************************************************************************
*/

void * allocPage(){
	int i, p_dir, p_table;
	unsigned int * ans;
	unsigned char j, probe;
	for (i=0; i < (PAGE_QTY/8); i++)
		for(j=0, probe=1; j < 8; j++, probe*=2)
			if ((__page_status[i] & probe) != probe){	// Page status -> available
				__page_status[i] |= probe;	// Set free page status as not available.
				p_dir = i << 22; printf("P_dir: %d\n", p_dir);
				p_table = j << 12; printf("P_table: %d\n", p_table);
				return (void *)(p_dir + p_table);
			}
	return (void *)NULL;	
}

__Process_pages allocProcess(int pid){
	__Process_pages p_pages;
	void * d_page = allocPage();
	void * s_page = allocPage();
	p_pages.pid = pid;
	p_pages.d_page = d_page;
	p_pages.s_page = s_page;
	__pages_per_process[pid] = p_pages;
	return p_pages;
}

void protect(int pid){
	int i, p_dir, p_table;
	unsigned int * page_table = (unsigned int *)(KERNEL_LIMIT);
	unsigned int * addr;
	
	// Sets pages as NOT-PRESENT
	for(i=0; i < (PAGE_DIR_QTY * 1024); i++)
		page_table[i] = page_table[i] & 0xFFFFFFFE;	// Sets present bit as not present
	
	// Set data page of process pid as available
	p_dir = (unsigned int)__pages_per_process[pid].d_page >> 22;
	p_table = (unsigned int)__pages_per_process[pid].d_page << 10 >> 22;
	addr = (unsigned int *)(PAGE_SIZE * p_dir + p_table * 4);
	*addr = *addr | P_FLAG;
	
	// Set stack page of process pid as available
	p_dir = (unsigned int)__pages_per_process[pid].s_page >> 22;
	p_table = (unsigned int)__pages_per_process[pid].s_page << 10 >> 22;
	addr = (unsigned int *)(PAGE_SIZE * p_dir + p_table * 4);
	*addr = *addr | P_FLAG;
}

// ELIMINAR CUANDO ELIMINE LOS TESTEOS DE ALLOCPAGE EN SHELL
void test(){
	char p = 254;
	__page_status[0] &= p;
}
/////////////////////////////////////////////////////////////

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


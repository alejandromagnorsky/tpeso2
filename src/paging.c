#include "../include/paging.h"
#include "../include/kasm.h"
#include "../include/shell.h"
#include "../include/console.h"


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
		__page_status[i] = /*( i < 512 ) ? 0xFF :*/ 0x00;
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
	int i, p_dir, p_table, p_number;
	unsigned int * ans;
	unsigned char j, probe;
	for (i=0; i < (PAGE_QTY/8); i++){
		for(j=0, probe=1; j < 8; j++, probe*=2){
			if ((__page_status[i] & probe) != probe){	// Page status -> available
				__page_status[i] |= probe;	// Set free page status as not available.
				p_dir = i << 22;
				p_table = j << 12;
				p_number = i * 8 + j;
				printf("Page number: %d\n", (p_number % 1024) );
				printf("Page virtual address: %d\n", ((p_number / 1024) << 22) + ((p_number % 1024) << 12));
				//printf("Page_number: %d\n", p_dir + p_table);
				aux = (void *)(((p_number / 1024) << 22) + ((p_number % 1024) << 12));
				printf("PRINT ADENTRO DE ALLOCPAGE: AUX: %d\n", aux);
				return (void *)(((p_number / 1024) << 22) + ((p_number % 1024) << 12));
			}
		}
	}
	printf("Returning null\n");
	return (void *)NULL;
}

__Process_pages allocProcess(int pid){
	__Process_pages p_pages;
	void * d_page = allocPage();
	void * s_page = allocPage();
	//printf("PRINT IN ALLOCPROCESS*********************\nDATA PAGE: %d\n", (unsigned int)d_page);
	//printf("STACK PAGE: %d\n", (unsigned int)s_page);
	p_pages.pid = pid;
	p_pages.d_page = d_page;
	p_pages.s_page = s_page;
	printf("PRINT IN ALLOCPROCESS********************************************\nDATA PAGE: %d\n", (unsigned int)p_pages.d_page);
	printf("STACK PAGE: %d\nEND PRINT IN ALLOCPROCESS******************\n", (unsigned int)p_pages.s_page);
	__pages_per_process[pid] = p_pages;
	return p_pages;
}

void breakProtection(){
	int i;
	unsigned int * page_table = (unsigned int *)(KERNEL_LIMIT);
	for(i=0; i < (PAGE_DIR_QTY * 1024); i++)
		page_table[i] = page_table[i] | P_FLAG;
}

void protect(){
/*	int pid = mt_curr_task->pid;
	int i, p_dir, p_table, d_page_number, s_page_number;
	unsigned int * page_table = (unsigned int *)(KERNEL_LIMIT);
	unsigned int s_page_dir, s_page_table, d_page_dir, d_page_table;
	
	d_page_dir = (unsigned int)__pages_per_process[pid].d_page >> 22;
	d_page_table = (unsigned int)__pages_per_process[pid].d_page << 10 >> 22;
	d_page_number = d_page_dir * 1024 + d_page_table;

	s_page_dir = (unsigned int)__pages_per_process[pid].s_page >> 22;
	s_page_table = (unsigned int)__pages_per_process[pid].s_page << 10 >> 22;
	s_page_number = s_page_dir * 1024 + s_page_table;
*/	
	// Protects kernel's 4M memory chunk
	// 4 is a magic number here
	//for(i=4; i < 1024; i++)
	//	page_table[i] = page_table[i] & 0xFFFFFFFE;
	
	// Skips first eight entries in page table 1 and continues protecting
/*	for(i=1024 + 8; i < PAGE_DIR_QTY * 1024; i++)
		if ( i != d_page_number && i != s_page_number)
			page_table[i] = page_table[i] & 0xFFFFFFFE;*/

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


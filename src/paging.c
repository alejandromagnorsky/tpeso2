#include "../include/allocator.h"
#include "../include/kasm.h"


/* ******************************************************************************************************
/* ******************************************************************************************************
	COSAS PARA TERMINAR DE PAGING.C:
		1) Agregar condicion en PAGING() para que al tener un MEMORY_SIZE variable, y por ejemplo sea
	mayor a 32M, se cambie automáticamente al siguiente PDE y se siga protegiendo las páginas restantes.
	
		2) Una vez que defiendo las páginas que usé para el KERNEL y los PAGE DIRECTORIES y TABLES,
	qué hago con las páginas disponibles para uso? Cómo las inicializo? Tengo una idea, pero quiero confirmar.
	
		3) Terminar la función MAP().
		
	COSAS PARA TERMINAR DE ALLOCATOR.C:
		1) Revisar DEALLOCATE().
		2) BITMAP? STACK? ambos? Decidir eso.

   ******************************************************************************************************
   ****************************************************************************************************** */
 
/* ****************************************************************************************************** */
/* ****************************************************************************************************** */
// INCLUDES TEMPORAL
#define EMPTY_ADDRESS 0
#define P_FLAG 1
#define RW_FLAG 2
#define U_FLAG 4
#define K_FLAG	( U_FLAG | RW_FLAG | P_FLAG )	// Supervisor level, read/write permissions, present.

unsigned int pages[PAGE_QTY/32];

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
	unsigned int * page_dir = (unsigned int *)(KERNEL_LIMIT & 0xFFFFF000);	// page_dir is now page-aligned.
	unsigned int * page_table = (unsigned int *)((unsigned int)page_dir + PAGE_SIZE);
	
	/*
	 * 	As we are working on a (virtual) 32M-memory hardware, we
	 * 	can use only 8 page directory entries to map all the memory.
	 */
	 
	/* FIRST ENTRY IN PAGE DIRECTORY */
	page_dir[0] = (unsigned int)page_table | K_FLAG;
	
	for(i=1; i < PAGE_SIZE/4; i++)				// Protects every entry in page directory from MMU,
		page_dir[i] = EMPTY_ADDRESS | RW_FLAG;	// although we will be using only the first one.
	
	/* Kernel page table. Maps first 4M of memory as kernel memory. */
	for(i=0; i < PAGE_SIZE/4; i++) page_table[i] = (unsigned int)allocator() | K_FLAG;
		
	
	/* SECOND ENTRY IN PAGE DIRECTORY */
	page_dir[1] = (unsigned int)page_table + PAGE_SIZE;
	page_table = (unsigned int *)((unsigned int)page_table + PAGE_SIZE);

/*	******************************************************************************************************
	***************************************** ( 1 ) ******************************************************
	******************************************************************************************************
*/	
															// PTx -> Page Tables pointed by PDEx (Page Directory Entry x)
	page_table[0] = (unsigned int)allocator() | K_FLAG;		// 1st entry in PT2 maps all page directory entries.
	for(i=0; i < MEMORY_LIMIT/0x400000 ; i++)
		page_table[i] = (unsigned int)allocator() | K_FLAG;	// (i+1)th entry in PT2 maps all 1024 page directory entries.
	
	
/*	******************************************************************************************************
	***************************************** ( 2 ) ******************************************************
	******************************************************************************************************
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
	int byte_ptr, bit_ptr;
	for(byte_ptr = 0; byte_ptr < (PAGE_QTY/32) && pages[byte_ptr] == 0xFFFFFFFF; byte_ptr++);
	
	if(byte_ptr == (PAGE_QTY/32)) return NULL;	// No page available.
	
	for(bit_ptr = 1; (~pages[byte_ptr] & bit_ptr) == 0; byte_ptr *= 2);
	
	unsigned int phys_base = (unsigned int)phys_addr & 0xFFFFF000;
	int phys_offset = (unsigned int)phys_addr & 0xFFF;
	
	unsigned int * virtual_addr = 0;
	int page_table = ((byte_ptr*32) + bit_ptr) / 1024;
	int page_frame = ((byte_ptr*32) + bit_ptr) % 1024;
	
}


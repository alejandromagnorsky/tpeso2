#include "../include/kasm.h"
#include "../include/kernel.h"
#include "../include/keyboard.h"
#include "../include/interrupts.h"

#include "../include/multiboot.h"
void __printMemoryMap(multiboot_info_t * mbd);

// INCLUDE TEMPORAL
// extern char page_map[];
////////////////////////

DESCR_INT idt[0x81];	/* IDT de 81h entradas*/
IDTR idtr;				/* IDTR */


int getSeconds(){
	_outport(0x70, 0);
	return _inport(0x71);
}

size_t __read(int fd, void* buffer, size_t count){
	int i;
	int * vec = buffer;
	for(i = 0; i <= count; i++){
		if( kbuffer.__KBUFFER_PTR_RD ==  kbuffer.__KBUFFER_PTR_WR )
			return i;
		vec[i] = kbuffer.buf[kbuffer.__KBUFFER_PTR_RD];
		kbuffer.__KBUFFER_PTR_RD = (kbuffer.__KBUFFER_PTR_RD+1) % KBUFFER_SIZE;
	}
	return i;
}

size_t __write(int fd, const void* buffer, size_t count){
	if(fd == stdout)
			__write_terminal(buffer,count);
	return count;
}




/**********************************************
kmain() 
Punto de entrada de C
*************************************************/

/* 'multiboot_info_t' stores data about memory map */
kmain(multiboot_info_t * mbd, unsigned int magic) 
{
	_Cli();

	initializePics();
    
	kbuffer.__KBUFFER_PTR_RD = 0;
	kbuffer.__KBUFFER_PTR_WR = 0;
	// First initialize terminals.
	__INIT_TTY();

	_turn_cursor_on();
	sysinfo();
	
	setupIDT();
	
	// Habilito interrupcion del timer tick y del teclado
	_mascaraPIC1(0xFC);
	_mascaraPIC2(0xFF);
	
	_Sti();
	
	// A TENER EN CUENTA: SI BARDEO UNA DIRECCION QUE NO EXISTE (COMO LA 0x5000000) IMPRIME -1. NO TIRA ERROR NI NADA.
	//unsigned int * ptr = (unsigned int *)0x500000;
	//*ptr = 5;
	//printf("%d\n", *ptr);
	paging();
	//printf("%d\n", *ptr);
	
	printf("\n");
	__printSystemSymbol();

	shell();
}

/*
 * Temporal function to understand what is going on!
 *
 * The following map was printed by __printMemoryMap(), running on a bochs emulator with 32M RAM.
 *
 * --- MEMORY MAP ------------------------------------------------------------------------------|
 * |	START	|	END		|	SIZE	|	DESCRIPTION											|
 * --- BEGINNING OF LOWER MEMORY ---------------------------------------------------------------|
 * |	0K		|	636K	|	636K	|	type 1												| ==>	Do NOT use first 1280 bytes although are type 1. Remember
 * |	636K	|	640K	|	4K		|	Extended BDA, type 2								| IVT is stored in the first 1K of memory, among others.
 * |	640K	|	928K	|	288K	|	Reserved memory area (unlisted, type 2)				| ==> Reserved memory area is for:
 * |	928K	|	1024K	|	96K		|	Reserved memory area (type 2)						| 	- System BIOS ROM
 * |			|			|			|														|	- Video RAM
 * --- BEGINNING OF UPPER MEMORY ---------------------------------------------------------------|	- BIOS Extension ROMs on plug in cards
 * |	1024K	|	32704K	|	31680K	|	type 1												|	- RAM on some I/O adapters
 * |	32704K	|	32768K	|	64K		|	type 3												|
 * ---------------------------------------------------------------------------------------------|
 *
 * Makes a total of 32 MiB, which is effectively what the bochsrc is showing in one of the first lines!!
 *
 * REGION TYPES:
 * 	- Type O: Negative.
 * 	- Type 1: Usable (normal) RAM.
 * 	- Type 2: Reserved - unusable.
 * 	- Type 3: ACPI reclaimable memory.
 * 	- Type 4: ACPI NVS memory.
 * 	- Type 5: Area containing bad memory.
 *
 * TIPS:
 *	- Type 3 "ACPI reclaimable" memory regions may be used like (and combined with) normal "available RAM" areas as long as you're finished using the ACPI
 *	tables that are stored there (i.e. it can be "reclaimed").
 *	- Types 2, 4, 5 (reserved, ACPI non-volatile, bad) mark areas that should be avoided when you are allocating physical memory.
 *	- Treat unlisted regions as Type 2 -- reserved. 
 */

void __printMemoryMap(multiboot_info_t * mbd){
	memory_map_t * mmap = (memory_map_t *)mbd->mmap_addr;
	int i = 0;

	printf("Lower Memory amnount: %d KiB\n", mbd->mem_lower);
	printf("Upper Memory amnount: %d KiB\n", mbd->mem_upper);
	printf("Memory map buffer address: %d - length: %d\n", mbd->mmap_addr, mbd->mmap_length);
	
	while( (unsigned long)mmap < mbd->mmap_addr + mbd->mmap_length){ // Leaves loop when it exceeds mmap buffer size
		printf("Memory Map: %d\n", i++);
		printf("\tSize: %d\n", mmap->size);
		printf("\tBase address: %d  %d\n", mmap->base_addr_high, mmap->base_addr_low);
		printf("\tBase address length: %d  %d\n", mmap->length_high, mmap->length_low);
		printf("\tType: %d\n", mmap->type);

		mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(unsigned int));
	}	
}


/* 
** Initialize the PICs and remap them
*/
void initializePics()
{
	/* Send ICW1 to both PICS 
	** ICW1 is used to tell the PICs if a ICW4 is following 
	** and if the PIC is working in a cascaded PIC environment
	*/

	_outport(0x20, 0x11);
	_outport(0xA0, 0x11);
	/* Send ICW2 to both PICS 
	** ICW2 tells the PICs where to map IRQ0 and IRQ8.
	*/
	_outport(0x21, 0x20);
	_outport(0xA1, 0x28);
	/* Send ICW3 to both PICS 
	** ICW3 is used for telling the PICs which IRQ to use for
	** the communication between each other
	*/
	_outport(0x21, 2);      // Set the master PIC's IRQ2 to be connected whit the slave PIC
	_outport(0xA1, 0);	// For the slave PIC, it means that it is the Slave0
	/* Send ICW4 to both PICS 
	** ICW4 is used for telling that we are working in a 80x86 architecture 
	** and if the interruption is handled automatically or if it needs help from software.
	*/
	_outport(0x21, 1);
	_outport(0xA1, 1);

}


void setupIDT(){
//Load exceptions
	//Divide by cero
	setup_IDT_entry (&idt[0x00], 0x08, (dword)&_int_00_hand, ACS_EXC, 0);
	//Debug exceptions
	setup_IDT_entry (&idt[0x01], 0x08, (dword)&_int_01_hand, ACS_EXC, 0);
	//Nonmaskable interrupt
	setup_IDT_entry (&idt[0x02], 0x08, (dword)&_int_02_hand, ACS_EXC, 0);
	//Breakpoint
	setup_IDT_entry (&idt[0x03], 0x08, (dword)&_int_03_hand, ACS_EXC, 0);
	//Overflow
	setup_IDT_entry (&idt[0x04], 0x08, (dword)&_int_04_hand, ACS_EXC, 0);
	//Bounds check
	setup_IDT_entry (&idt[0x05], 0x08, (dword)&_int_05_hand, ACS_EXC, 0);
	//Invalid opcode
	setup_IDT_entry (&idt[0x06], 0x08, (dword)&_int_06_hand, ACS_EXC, 0);
	//Coprocessor not available
	setup_IDT_entry (&idt[0x07], 0x08, (dword)&_int_07_hand, ACS_EXC, 0);
	//Double fault
	setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand, ACS_EXC, 0);
	//Invalid TSS
	setup_IDT_entry (&idt[0x0A], 0x08, (dword)&_int_0A_hand, ACS_EXC, 0);
	//Segment not present
	setup_IDT_entry (&idt[0x0B], 0x08, (dword)&_int_0B_hand, ACS_EXC, 0);
	//Stack exception
	setup_IDT_entry (&idt[0x0C], 0x08, (dword)&_int_0C_hand, ACS_EXC, 0);
	//General protection
	setup_IDT_entry (&idt[0x0D], 0x08, (dword)&_int_0D_hand, ACS_EXC, 0);
	//Page fault
	setup_IDT_entry (&idt[0x0E], 0x08, (dword)&_int_0E_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x0F], 0x08, (dword)&_int_0F_hand, ACS_EXC, 0);
	//Coprocessor error
	setup_IDT_entry (&idt[0x10], 0x08, (dword)&_int_10_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x11], 0x08, (dword)&_int_11_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x12], 0x08, (dword)&_int_12_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x13], 0x08, (dword)&_int_13_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x14], 0x08, (dword)&_int_14_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x15], 0x08, (dword)&_int_15_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x16], 0x08, (dword)&_int_16_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x17], 0x08, (dword)&_int_17_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x18], 0x08, (dword)&_int_18_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x19], 0x08, (dword)&_int_19_hand, ACS_EXC, 0);
	//Reserved
	setup_IDT_entry (&idt[0x1A], 0x08, (dword)&_int_1A_hand, ACS_INT, 0);
	//Reserved
	setup_IDT_entry (&idt[0x1B], 0x08, (dword)&_int_1B_hand, ACS_INT, 0);
	//Reserved
	setup_IDT_entry (&idt[0x1C], 0x08, (dword)&_int_1C_hand, ACS_INT, 0);
	//Reserved
	setup_IDT_entry (&idt[0x1D], 0x08, (dword)&_int_1D_hand, ACS_INT, 0);
	//Reserved
	setup_IDT_entry (&idt[0x1E], 0x08, (dword)&_int_1E_hand, ACS_INT, 0);
	//Reserved
	setup_IDT_entry (&idt[0x1F], 0x08, (dword)&_int_1F_hand, ACS_INT, 0);


/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */
//Rutina de atención del timer tick
	setup_IDT_entry (&idt[0x20], 0x08, (dword)&_int_20_hand, ACS_INT, 0);

//Rutina de atención del teclado
	setup_IDT_entry (&idt[0x21], 0x08, (dword)&_int_21_hand, ACS_INT, 0);	

//Rutina de atención de escritura y lectura
	setup_IDT_entry (&idt[0x80], 0x08, (dword)&_int_80_hand, ACS_INT, 0);

/* Carga de IDTR    */

	idtr.base = 0;  
	idtr.base +=(dword) &idt;
	idtr.limit = sizeof(idt)-1;
	
	_lidt (&idtr);
}



/*
--------------------------------------------------------------------------------
block - bloquea un proceso
--------------------------------------------------------------------------------
*/

void
block(Task * task, TaskState state)
{
	mt_dequeue(task);
	mt_dequeue_time(task);
	task->state = state;
}


/*
--------------------------------------------------------------------------------
msecs_to_ticks, ticks_to_msecs - conversion de milisegundos a ticks y viceversa
--------------------------------------------------------------------------------
*/

int
msecs_to_ticks(int msecs)
{
	return (msecs + MSPERTICK - 1) / MSPERTICK;
}

int
ticks_to_msecs(int ticks)
{
	return ticks * MSPERTICK;
}



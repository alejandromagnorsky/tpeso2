#include "../include/kasm.h"
#include "../include/defs.h"
#include "../include/kernel.h"
#include "../include/kc.h"
#include "../include/keyboard.h"
#include "../include/interrupts.h"

DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
IDTR idtr;				/* IDTR */


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

kmain() 
{
	initializePics();
      	kbuffer.__KBUFFER_PTR_RD = 0;
	kbuffer.__KBUFFER_PTR_WR = 0;
	// First initialize terminals.
	__INIT_TTY();

	_turn_cursor_on();
	sysinfo();
	__printSystemSymbol();


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

	_Cli();

/* Habilito interrupcion del teclado */

	_mascaraPIC1(0xFD);
	_mascaraPIC2(0xFF);

	_Sti();

	shell();
}


#include "../include/kasm.h"
#include "../include/kernel.h"
#include "../include/keyboard.h"
#include "../include/interrupts.h"

#include "../include/multiboot.h"
void __printMemoryMap(multiboot_info_t * mbd);
void printA();
void printB();

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
	DisableInts();

	initializePics();
    
	kbuffer.__KBUFFER_PTR_RD = 0;
	kbuffer.__KBUFFER_PTR_WR = 0;
	// First initialize terminals.
	__INIT_TTY();

	_turn_cursor_on();
	
	
	setupIDT();

	// Habilito interrupcion del timer tick y del teclado
	_mascaraPIC1(0xFC);

	_mascaraPIC2(0xFF);

	RestoreInts();


	printf("EMPEZE\n\n\n");

	sysinfo();

	/*
	__asm__("pushf ; cli");
	
	func(0);
	__asm__("popf");
	*/
	

	printf("CS: %d\n", _read_cs());
	printf("DS: %d: EDX: %d\n", _read_ds(), _read_edx());
	printf("SS: %d : SP : %d\n", _read_ss(), _read_sp());

	// A TENER EN CUENTA: SI BARDEO UNA DIRECCION QUE NO EXISTE (COMO LA 0x5000000) IMPRIME -1. NO TIRA ERROR NI NADA.
	//unsigned int * ptr = (unsigned int *)0x500000;
	//*ptr = 5;
	//printf("%d\n", *ptr);
	paging();
	//printf("%d\n", *ptr);
	
	printf("\n");
	__printSystemSymbol();


	createTask(printA, 1000, NULL, 1);
	createTask(printB, 1000, NULL, 2);

	shell();
}

void printA(){
	printf("A");
}

void printB(){
	printf("B");
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
ready - desbloquea un proceso y lo pone en la cola de ready

Si el proceso estaba bloqueado en WaitQueue, Send o Receive, el argumento
success determina el status de retorno de la funcion que lo bloqueo.
--------------------------------------------------------------------------------
*/

void
__ready(Task * task, bool success)
{
	if ( task->state == READY )
		return;

	mt_dequeue(task);
	mt_dequeue_time(task);
	mt_enqueue(task, &ready_q);
	task->success = success;
	task->state = READY;
}



Task *
createTask(TaskFunc func, unsigned stacksize, char * name, unsigned priority)
{
	//Task * task;
	Task task;
	static char v1[500];
	static char v2[500];

	/* ajustar tamano del stack y redondear a numero par */
	//stacksize = even(stacksize + mt_reserved_stack);


	/* alocar estructura del proceso */
	//task = Malloc(sizeof(Task));

	//task->name = task->send_queue.name = StrDup(name);
    	//task->priority = priority;
	task.priority = priority;
	//task->stack = Malloc(stacksize);
	if(priority == 1)
		task.stack = v1;
	else
		task.stack = v2;

	/* inicializar stack */
	
	task.ss = _read_ds();
	task.esp = _init_stack(func, task.stack, exit, INIFL);

//	return task;
	return NULL;
}


/*
--------------------------------------------------------------------------------
exit - finaliza el proceso actual

Todos los procesos creados con CreateTask retornan a esta funcion que los mata.
Esta funcion nunca retorna.
--------------------------------------------------------------------------------
*/


void
exit(void)
{
	deleteTask(mt_curr_task);
}



/*
--------------------------------------------------------------------------------
deleteTask - elimina un proceso creado con createTask

Si es el proceso actual, envia un mensaje al proceso de limpieza y se bloquea
como terminado.
--------------------------------------------------------------------------------
*/

void
free_task(Task * task)
{
	/*
	free(task->name);
	free(task->stack);
	free(task);
	*/
}

void
deleteTask(Task * task)
{
	if ( task == &main_task )
		printf("Imposible eliminar el proceso principal\n");

	flushQueue(&task->send_queue, false);
	DisableInts();
	if ( task == mt_curr_task )
	{
		mt_curr_task->state = TERMINATED;
		mt_enqueue(mt_curr_task, &terminated_q);
		scheduler();
	}
	else
	{
		block(task, TERMINATED);
		free_task(task);
	}
	RestoreInts();
}

/*
--------------------------------------------------------------------------------
free_terminated - elimina las tareas terminadas.
--------------------------------------------------------------------------------
*/

void
free_terminated(void)
{
	Task * task;

	while ( task = mt_getlast(&terminated_q) )
		free_task(task);
}

/*
--------------------------------------------------------------------------------
currentTask - retorna un puntero al proceso actual.
--------------------------------------------------------------------------------
*/

Task *
currentTask(void)
{
	return mt_curr_task;
}

/*
--------------------------------------------------------------------------------
pause - suspende el proceso actual
--------------------------------------------------------------------------------
*/

void
pause(void)
{
	suspend(mt_curr_task);
}


/*
--------------------------------------------------------------------------------
yield - cede voluntariamente la CPU
--------------------------------------------------------------------------------
*/

void
yield(void)
{
	ready(mt_curr_task);
}


/*
--------------------------------------------------------------------------------
delay - pone al proceso actual a dormir durante una cantidad de milisegundos
--------------------------------------------------------------------------------
*/

void
delay(int msecs)
{
	DisableInts();
	if ( msecs )
	{
		block(mt_curr_task, DELAYING);
		//if ( msecs != FOREVER )
			mt_enqueue_time(mt_curr_task, msecs_to_ticks(msecs));
	}
	else
		__ready(mt_curr_task, false);
	scheduler();
	RestoreInts();
}


/*
--------------------------------------------------------------------------------
suspend - suspende un proceso
--------------------------------------------------------------------------------
*/

void
suspend(Task * task)
{
	DisableInts();
	block(task, SUSPENDED);
	if ( task == mt_curr_task )
		scheduler();
	RestoreInts();
}

/*
--------------------------------------------------------------------------------
Ready - pone un proceso en la cola ready
--------------------------------------------------------------------------------
*/
void
ready(Task * task)
{
	DisableInts();
	__ready(task, false);
	scheduler();
	RestoreInts();
}

/*
--------------------------------------------------------------------------------
getPriority - retorna la prioridad de un proceso
--------------------------------------------------------------------------------
*/

unsigned
getPriority(Task * task)
{
	return task->priority;
}


/*
--------------------------------------------------------------------------------
setPriority - establece la prioridad de un proceso

Si el proceso estaba en una cola, lo desencola y lo vuelve a encolar para
reflejar el cambio de prioridad en su posición en la cola.
Si se le ha cambiado la prioridad al proceso actual o a uno que esta ready se
llama al scheduler.
--------------------------------------------------------------------------------
*/

void		
setPriority(Task * task, unsigned priority)
{
	TaskQueue * queue;

	DisableInts();
	task->priority = priority;
	if ( queue = task->queue )
	{
		mt_dequeue(task);
		mt_enqueue(task, queue);
	}
	if ( task == mt_curr_task || task->state == READY )
		scheduler();
	RestoreInts();
}


/*
--------------------------------------------------------------------------------
mt_select_task - determina el proximo proceso a ejecutar.

Retorna true si ha cambiado el proceso en ejecucion.
Llamada desde scheduler() y cuanto retorna una interrupcion de primer nivel.
Guarda y restaura el contexto del coprocesador y el contexto propio del usuario,
si existe. 
--------------------------------------------------------------------------------
*/

bool 
mt_select_task(void)
{
	Task * ready_task;

	/* Ver si el proceso actual puede conservar la CPU */
	if ( mt_curr_task->state == CURRENT )
	{
		if ( mt_curr_task->atomic_level )		/* No molestar */
			return false;

		/* Analizar prioridades y ranura de tiempo */
		ready_task = mt_peeklast(&ready_q);
		if ( !ready_task || ready_task->priority < mt_curr_task->priority ||
				ticks_to_run && ready_task->priority == mt_curr_task->priority )
			return false; 

		/* El proceso actual pierde la CPU */
		__ready(mt_curr_task, false);
	}

	/* Obtener el proximo proceso */
	last_task = mt_curr_task;
	mt_curr_task = mt_getlast(&ready_q);
	mt_curr_task->state = CURRENT;

	/* Si es el mismo de antes, no hay nada mas que hacer */
	if ( mt_curr_task == last_task )
		return false;

	/* Inicializar ranura de tiempo */
	ticks_to_run = QUANTUM;
	return true;
}

/*
--------------------------------------------------------------------------------
scheduler - selecciona el proximo proceso a ejecutar.
--------------------------------------------------------------------------------
*/

int
scheduler(void)
{
	mt_select_task();
	last_task->esp = _read_sp();
	return mt_curr_task->esp;			
}



/*
--------------------------------------------------------------------------------
Atomic - deshabilita el modo preemptivo para el proceso actual (anidable)
--------------------------------------------------------------------------------
*/

void
atomic(void)
{
	++mt_curr_task->atomic_level;
}

/*
--------------------------------------------------------------------------------
Unatomic - habilita el modo preemptivo para el proceso actual (anidable)
--------------------------------------------------------------------------------
*/

void
unatomic(void)
{
	if ( mt_curr_task->atomic_level && !--mt_curr_task->atomic_level )
	{
		DisableInts();
		scheduler();
		RestoreInts();
	}
}



/*
--------------------------------------------------------------------------------
createQueue - crea una cola de procesos
--------------------------------------------------------------------------------
*/
/*
TaskQueue *	
createQueue(char * name)
{
	
	TaskQueue * queue = Malloc(sizeof(TaskQueue));

	queue->name = StrDup(name);
	return queue;
}

*/
/*
--------------------------------------------------------------------------------
DeleteQueue - destruye una cola de procesos
--------------------------------------------------------------------------------
*/
/*
void
DeleteQueue(TaskQueue * queue)
{
	flushQueue(queue, false);
	Free(queue->name);
	Free(queue);
}
*/


/*
--------------------------------------------------------------------------------
WaitQueue, WaitQueueCond, WaitQueueTimed - esperar en una cola de procesos

El valor de retorno es true si el proceso fue despertado por SignalQueue
o el valor pasado a FlushQueue.
Si msecs es FOREVER, espera indefinidamente. Si msecs es cero, retorna false.
--------------------------------------------------------------------------------
*/
/*
bool			
waitQueue(TaskQueue * queue)
{
	return waitQueueTimed(queue, FOREVER);
}

bool			
waitQueueTimed(TaskQueue * queue, int msecs)
{
	bool success;

	if ( !msecs )
		return false;

	DisableInts();
	block(mt_curr_task, WAITING);
	mt_enqueue(mt_curr_task, queue);
	if ( msecs != FOREVER )
		mt_enqueue_time(mt_curr_task, msecs_to_ticks(msecs));
	scheduler();
	success = mt_curr_task->success;
	RestoreInts();

	return success;
}
*/
/*
--------------------------------------------------------------------------------
SignalQueue, FlushQueue - funciones para despertar procesos en una cola

SignalQueue despierta el ultimo proceso de la cola (el de mayor prioridad o
el que llego primero entre dos de la misma prioridad), el valor de retorno 
es true si desperto a un proceso. Este proceso completa su WaitQueue() 
exitosamente.
FlushQueue despierta a todos los procesos de la cola, que completan su
WaitQueue() con el resultado que se pasa como argumento.
--------------------------------------------------------------------------------
*/
/*
bool		
signalQueue(TaskQueue * queue)
{
	Task * task;

	DisableInts();
	if ( task = mt_getlast(queue) )
	{
		__ready(task, true);
		scheduler();
	}
	RestoreInts();

	return task != NULL;
}
*/
void			
flushQueue(TaskQueue * queue, bool success)
{
	Task * task;

	DisableInts();
	if ( mt_peeklast(queue) )
	{
		while ( task = mt_getlast(queue) )
			__ready(task, success);
		scheduler();
	}
	RestoreInts();
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



#include "../include/kasm.h"
#include "../include/kernel.h"
#include "../include/keyboard.h"
#include "../include/interrupts.h"
#include "../include/shell.h"
#include "../include/multiboot.h"
#include "../include/process.h"
#include "../include/console.h"
#include "../include/programs.h"
#include "../include/history.h"

void __printMemoryMap(multiboot_info_t * mbd);
void printA(int argc, char * argv[]);
void printB(int argc, char * argv[]);

// INCLUDE TEMPORAL
// extern char page_map[];
////////////////////////


int getSeconds(){
	_outport(0x70, 0);
	return _inport(0x71);
}

size_t __read(int fd, void* buffer, size_t count){

	while( fd != stdin || procStdin != __TTY_INDEX)
		yield();

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

	// If it is disabled
	if(procStdout == -1)
		return 0;


	int tmp = __TTY_INDEX;
	if(procStdout == __TTY_INDEX)
		__write_terminal(buffer,count, 1);	
	else {
		__switch_terminalnf(procStdout);
		__write_terminal(buffer,count,0);
	}
	__switch_terminalnf(tmp);

	return count;
}

void initializeShellCommands(){

	__QTY_PROGRAMS = 0;
	
	// Register of various functions
	__register_program("echo", echo);
	__register_program("clear", clear);
	__register_program("help", help);
	__register_program("man", man);
	__register_program("gcc", gcc);
	__register_program("tty", tty);
	__register_program("time", time);
	__register_program("arnold", arnold);
	__register_program("mkexc", mkexc);
	__register_program("cpuid", call_cpuid);
	__register_program("bingo", bingo);
	__register_program("reboot", reboot);
	__register_program("history", history);
	__register_program("top", top);
	__register_program("pstree", pstree);
	__register_program("kill", guikill);
	__register_program("ps", guips);
	__register_program("sleep", daemon1);
	__register_program("shell", (int (*)(int,char**))shell);
	__register_program("exit", (int (*)(int,char**))exit);
	__register_program("waitDead", (int (*)(int,char**))do_nothing);
	__register_program("getJoke", getJoke);

	__register_man_page("echo","Prints the string received.");
	__register_man_page("clear", "Clears the screen.");
	__register_man_page("help", "Prints all the possible commands known.");
	__register_man_page("man", "Shows the manual for any program");
	__register_man_page("gcc", "GNU C Compiler.");
	__register_man_page("tty", "Interface for changing terminal settings. \n"  \
				"Arguments: \n" \
				" \t [-s terminal_index] | _Switches terminal \n"
				" \t [-l] | Switches to the _last terminal \n"
				" \t [-n] | Switches to the _next terminal \n"
				" \t [-ss string] | Changes the _system _symbol to string \n"
				" \t [-c foreground background] | Changes terminal _color.");
	__register_man_page("time","Prints hour, minutes and seconds.");
	__register_man_page("arnold","Arnold Alois Schwarzenegger, as John Matrix in Commando(1985)");
	__register_man_page("mkexc","Generates the exception corresponding to the second argument." \
				     "Valid values are numbers between 0 and 31.");
	__register_man_page("bingo","Bingo for two players.");
	__register_man_page("reboot","Reboots the system.");
	__register_man_page("history","Shows the shell history.");
	__register_man_page("top","Shows CPU resources.");
	__register_man_page("pstree","Shows process tree.");
	__register_man_page("shell","Opens a new shell.");
	__register_man_page("exit","Exits from shell.");
	__register_man_page("kill","-pid Kills a process with pid");
	__register_man_page("sleep","n Sleep n seconds");
	__register_man_page("ps","Snapshot of current processes. -e for all processes");
	__register_man_page("waitDead","Demonstration only: makes shell wait first dead children");
	__register_man_page("getJoke","Get a joke from the jokes server. Use a number between 0 and 9");
	
}


/**********************************************
kmain() 
Punto de entrada de C
*************************************************/

/* 'multiboot_info_t' stores data about memory map */
int kmain(multiboot_info_t * mbd, unsigned int magic) 
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
	

	// A TENER EN CUENTA: SI BARDEO UNA DIRECCION QUE NO EXISTE (COMO LA 0x5000000) IMPRIME -1. NO TIRA ERROR NI NADA.
	//unsigned int * ptr = (unsigned int *)0x500000;
	//*ptr = 5;
	//printf("%d\n", *ptr);
	paging();
	//printf("%d\n", *ptr);

	initializeShellCommands();
	__init_history();


	_srand(1295872354);

	// Inicializar multitasker queues
	mt_initTaskArray( __taskArray, __MAX_TASKS);
	mt_initTaskQueue( &ready_q, "Ready Queue");
	mt_initTaskQueue( &blocked_q, "Blocked Queue");

	// Inicializar procesos e init (aca esta el fork inicial)
	__initializeProcessSubSystem();

	/* Inicializar proceso principal */
	memcpy(main_task.name,"Main Task", 10);
	main_task.state = CURRENT;
	main_task.priority = 0;
	main_task.send_queue.name = main_task.name;
	main_task.ss = _read_ds();
	main_task.esp = _init_stack(do_nothing, main_task.stack+STACKSIZE-1, exit, INIFL, 0, NULL);


	ticks_to_run = QUANTUM;
	mt_curr_task = &main_task;

	RestoreInts();

	do_nothing(0, NULL);

	return 0;
}

void do_nothing(int argc, char * argv[]){ 
	while(true)
		;
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



void
memcpy(char * out, char * src, int length){	
	int i;
	for(i = 0; i < length; i++){
		out[i] = src[i];
	}
}


Task *
createTask(TaskFunc func, int argc, char * argv[], char * name, unsigned priority, int pid)
{
	//Task * task;
	Task * task = mt_getAvailableTask( __taskArray, __MAX_TASKS);
	task->priority = priority;
	memcpy(task->name, name,strlen(name)+1);
	task->count = 0;
	task->pid = pid;

	task->ss = _read_ds();
	task->esp = _init_stack(func, task->stack+STACKSIZE-1, exit, INIFL, argc, argv);

	return task;
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
		mt_curr_task->exists = 0;
		//mt_curr_task->state = TERMINATED;
		//mt_enqueue(mt_curr_task, &terminated_q);
		//scheduler();
	}
	else
	{
		task->exists = 0;
		mt_dequeue(task);
//		block(task, TERMINATED);
//		free_task(task);
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

	while ( (task = mt_getlast(&terminated_q)) )
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
block - bloquea un proceso
--------------------------------------------------------------------------------
*/

void
block(Task * task, TaskState state)
{
	mt_dequeue(task);
	mt_enqueue(task, &blocked_q);
	task->state = state;
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
		_int_20_call(0);
	RestoreInts();
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
	mt_enqueue(task, &ready_q);
	task->success = success;
	task->state = READY;
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
	_int_20_call(0);
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

void setPriority(Task * task, unsigned priority){
	TaskQueue * queue;

	DisableInts();
	task->priority = priority;
	if ( (queue = task->queue) )
	{
		mt_dequeue(task);
		mt_enqueue(task, queue);
	}
	if ( task == mt_curr_task || task->state == READY )
		_int_20_call(0);
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



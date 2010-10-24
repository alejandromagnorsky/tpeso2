#ifndef _kernel_
#define _kernel_
#include "../include/defs.h"
#include "../include/mtask.h"
#include "../include/queue.h"

/********************************** 
*
*  Kernel
*
***********************************/



#define OS_PID	0
#define stdout 1
#define stdin 0
#define KBUFFER_SIZE 128

#define NULL_STACK		256 			/* tamano de stack para proceso nulo */ 
#define INIFL			0x200
#define STACKSIZE 		512
#define QUANTUM			2
#define MSPERTICK 		55
#define	DisableInts()	__asm__ ( "cli" )
#define	RestoreInts()	__asm__ ( "sti" )

typedef struct{
	int buf[KBUFFER_SIZE];
	int __KBUFFER_PTR_RD;
	int __KBUFFER_PTR_WR;
	char flag;
}__keyboard_buffer;

__keyboard_buffer kbuffer;

int (*player)(void);
int getSeconds();

typedef int size_t;
typedef short int ssize_t;
typedef enum eINT_80 {WRITE=0, READ} tINT_80;
typedef enum eUSER {U_KERNEL=0, U_NORMAL} tUSERS;

/* __write
*
* Recibe como parametros:
* - File Descriptor
* - Buffer a donde escribir
* - Cantidad
*
**/
size_t __write(int fd, const void* buffer, size_t count);

/* __read
*
* Recibe como parametros:
* - File Descriptor
* - Buffer del source
* - Cantidad
*
**/
size_t __read(int fd, void* buffer, size_t count);


void setupIDT();
void initializePics();

Task * mt_curr_task;		/* proceso en ejecucion */
Task * last_task;			/* proceso anterior */
Task main_task;				/* proceso principal */
int ticks_to_run;			/* ranura de tiempo */
TaskQueue ready_q;			/* cola de procesos ready */
TaskQueue terminated_q;		/* cola de procesos terminados */

void block(Task * task, TaskState state);
void __ready(Task * task, bool success);
void free_task(Task * task);

int msecs_to_ticks(int msecs);
int ticks_to_msecs(int ticks);

void free_terminated(void);		/* libera tareas terminadas */
void do_nothing();		/* funcion del proceso nulo */

#endif

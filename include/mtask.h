#ifndef _mtask_
#define _mtask_

#include "../include/defs.h"

typedef enum {SUSPENDED, READY, CURRENT, DELAYING, WAITING, SENDING, RECEIVING, TERMINATED} TaskState;
typedef struct Task Task;

typedef struct{
	char *	name;
	Task *	head;
	Task *	tail;

} TaskQueue;

struct Task{
	char * 	name;
	int		pid;
	unsigned 	priority;
	unsigned	atomic_level;
	TaskState state;	

	char * 	stack;
	int		ss;
	int		esp;
	
	// Para la cola en la que esta el proceso
	TaskQueue *	queue; 	// Cola de procesos en la que se encuentra el proceso
	Task *	prev;		// El task anterior a este en la cola de procesos especifica
	Task *	next;		// El task siguiente a este en la cola de procesos especifica
	
	// Para cuando usa sleep
	int		ticks;	// Ticks restantes para ser despertado
	bool	in_time_q;		// Si es verdadero, el proceso esta en la cola del tiempo
	Task *	time_prev;		// El task anterior a este en la cola del tiempo
	Task *	time_next;		// El task siguiente a este en la cola del tiempo
	
	// Para los IPCs
	bool	success;		// Indica si el mensaje fue recibido o enviado
	Task *	from;			// Indica el emisor del ultimo mensaje recibido
	char *	msg;			// Mensaje a mandar o recibido
	int		msg_size; 		// Tamaño del mensaje
	TaskQueue	send_queue;	// La cola donde los demas procesos esperan para dejarle un mensaje a este
};


typedef struct
{
	unsigned 			ebp, edi, esi;
	unsigned short		ds, es, cs;
	unsigned 			edx, ecx, ebx, eax, eip, eflags;
}
WordRegs;

typedef struct
{
	unsigned short		bp, di, si, ds, es;
	unsigned char		dl, dh, cl, ch, bl, bh, al, ah;
	unsigned short		ip, cs, flags;
}
ByteRegs;

typedef union
{
	WordRegs	x;
	ByteRegs	h;
}
InterruptRegs;


typedef struct
{
	InterruptRegs   regs;
	void			(*retaddr)(void);
	void *			arg;
}
InitialStack;


// Puntero a la funcion a ejecutar al iniciar el proceso
typedef void (*TaskFunc)(void * arg);

Task *		createTask(TaskFunc func, unsigned stacksize, void * arg, char * name, unsigned priority);
Task *		currentTask(void);
void		deleteTask(Task * task);

unsigned	getPriority(Task * task);
void		setPriority(Task * task, unsigned priority);
void		atomic(void);
void		unatomic(void);
void		suspend(Task * task);
void		ready(Task * task);

TaskQueue *	createQueue(char *name);
void		deleteQueue(TaskQueue * queue);
bool		waitQueue(TaskQueue *queue);
bool		waitQueueTimed(TaskQueue * queue, int msecs);
bool		signalQueue(TaskQueue * queue);
void		flushQueue(TaskQueue * queue, bool success);

bool		send(Task * to, char * msg, int size);
bool		receive(Task ** from, char * msg, int * size);

void		pause(void);
void		yield(void);
void		delay(int msecs);
void		exit(void);


#endif

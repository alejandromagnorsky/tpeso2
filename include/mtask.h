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
	int 	priority;
	TaskState state;	

	char * 	stack;
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
	TaskQueue	sendQueue;	// La cola donde los demas procesos esperan para dejarle un mensaje a este
};


// Puntero a la funcion a ejecutar al iniciar el proceso
typedef void (*TaskFunc)(void * arg);

Task *		createTask(TaskFunc func, unsigned stacksize, void * arg, char * name, int priority);
Task *		currentTask(void);
void		deleteTask(Task * task);

int			getPriority(Task * task);
void		setPriority(Task * task, int priority);
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

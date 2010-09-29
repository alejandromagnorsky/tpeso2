#ifndef _mtask_
#define _mtask_

#include "../include/defs.h"

typedef enum {TaskSuspended, TaskReady, TaskCurrent, TaskDelaying, TaskWaiting, TaskSending, TaskReceiving, TaskTerminated} TaskState;
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
	
	char * 	stack;
	int		esp;
	
	// Para la cola en la que esta el proceso
	TaskQueue *	taskqueue; 	// Cola de procesos en la que se encuentra el proceso
	Task *	task_prev;		// El task anterior a este en la cola de procesos especifica
	Task *	task_next;		// El task siguiente a este en la cola de procesos especifica
	
	// Para cuando usa sleep
	int		ticksToWait;	// Ticks restantes para ser despertado
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

Task *		CreateTask(TaskFunc func, unsigned stacksize, void * arg, char * name, int priority);
Task *		CurrentTask(void);
void		DeleteTask(Task * task);

int			GetPriority(Task * task);
void		SetPriority(Task * task, int priority);
void		Suspend(Task * task);
void		Ready(Task * task);

TaskQueue *	CreateQueue(char *name);
void		DeleteQueue(TaskQueue * queue);
bool		WaitQueue(TaskQueue *queue);
bool		WaitQueueTimed(TaskQueue * queue, int msecs);
bool		SignalQueue(TaskQueue * queue);
void		FlushQueue(TaskQueue * queue, bool success);

bool		Send(Task * to, char * msg, int size);
bool		Receive(Task ** from, char * msg, int * size);

void		Pause(void);
void		Yield(void);
void		Delay(int msecs);
void		Exit(void);
#endif

#ifndef _mtask_
#define _mtask_

#include "../include/defs.h"

#define STACKSIZE 		4096
#define MAX_PRIO		4
#define MIN_PRIO        0
#define MAX_NAMELENGTH 32
typedef enum {SUSPENDED, READY, CURRENT, DELAYING, WAITING, SENDING, RECEIVING, TERMINATED} TaskState;
typedef struct Task Task;

typedef struct{
	char *	name;
	Task *	head;
	Task *	tail;
	int iterations;
} TaskQueue;

struct Task{
	char name[MAX_NAMELENGTH];
	unsigned 	priority;
	TaskState state;	

	int count;			// La cantidad de veces que tuvo procesador desde que esta despierto
	int exists;			// Flag de existencia

	int pid;				// Useful for knowing its wrapper process

	char * stack;
	int	ss;
	int	esp;
	
	// Para la cola en la que esta el proceso
	TaskQueue *	queue; 	// Cola de procesos en la que se encuentra el proceso
	Task *	prev;		// El task anterior a este en la cola de procesos especifica
	Task *	next;		// El task siguiente a este en la cola de procesos especifica
	
	// Para los IPCs
	bool	success;		// Indica si el mensaje fue recibido o enviado
	Task *	from;			// Indica el emisor del ultimo mensaje recibido
	char *	msg;			// Mensaje a mandar o recibido
	int		msg_size; 		// Tamaño del mensaje
	TaskQueue	send_queue;	// La cola donde los demas procesos esperan para dejarle un mensaje a este
};



// Puntero a la funcion a ejecutar al iniciar el proceso
typedef void (*TaskFunc)(int argc, char * argv[]);


Task * mt_getAvailableTask( Task tasks[], int size);
Task *		createTask(TaskFunc func, int argc, char * argv[], char * name, unsigned priority, int pid);
Task *		currentTask(void);
void		deleteTask(Task * task);

unsigned	getPriority(Task * task);
void		setPriority(Task * task, unsigned priority);
void		suspend(Task * task);
void		ready(Task * task);
void		yield(void);
void		exit(int argc, char * argv[]);

bool		send(Task * to, char * msg, int size);
bool		receive(Task ** from, char * msg, int * size);

#endif

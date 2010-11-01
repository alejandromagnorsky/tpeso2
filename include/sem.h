#ifndef _sem_
#define _sem_

#include "../include/mtask.h"
#include "../include/kernel.h"
#include "../include/queue.h"

#define MAX_MSGLENGTH 150
#define MAX_SEMS 10

typedef struct{
	unsigned	value;
	TaskQueue *	queue;
} Semaphore;

typedef struct IPCPage {
	Semaphore semaphores[MAX_SEMS];
	TaskQueue semQueues[MAX_SEMS];
	bool semUsed[MAX_SEMS];
	char shm[MAX_MSGLENGTH];
} IPCPage;

extern IPCPage ipcpage;

Semaphore *	createSem(char * name, unsigned value);
void 		deleteSem(Semaphore * sem);
bool 		down(Semaphore * sem);
void 		up(Semaphore * sem);

#endif

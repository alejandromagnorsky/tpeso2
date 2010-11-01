#include "../include/sem.h"



IPCPage ipcpage;

void initIPC(){
	int i;
	for(i=0;i<MAX_SEMS;i++)
		ipcpage.semUsed[i] = false;
}

/*
--------------------------------------------------------------------------------
createSem - Devuelve un semaforo y establece su cuenta inicial
--------------------------------------------------------------------------------
*/

Semaphore * createSem(char * name, unsigned value)
{
	int i = 0;

	DisableInts();

	while(ipcpage.semUsed[i] && i < MAX_SEMS)
		i++;
	if(i == MAX_SEMS)
		return NULL;
	ipcpage.semUsed[i] = true;

	RestoreInts();

	Semaphore * sem = &ipcpage.semaphores[i];
	
	mt_initTaskQueue(&ipcpage.semQueues[i], "");
	sem->queue = &ipcpage.semQueues[i];
	sem->value = value;
	return sem;
}

/*
--------------------------------------------------------------------------------
deleteSem - Libera un semaforo
--------------------------------------------------------------------------------
*/

void
deleteSem(Semaphore * sem){
	int i;
	for(i = 0; i < MAX_SEMS; i++)
		if(&ipcpage.semaphores[i] == sem){
			ipcpage.semUsed[i] = false;
			return;
		}
}

/*
--------------------------------------------------------------------------------
down - Decrementa el valor de un semaforo si este es mayor a 0. Sino blockea
 al proceso
--------------------------------------------------------------------------------
*/

bool
down(Semaphore * sem){
	bool success;

	DisableInts();
	if ( success = (sem->value > 0) )
		sem->value--;
	else
		success = waitQueue(sem->queue);
	RestoreInts();

	return success;
}

/*
--------------------------------------------------------------------------------
up - Despierta al primer proceso de la cola o incrementa la cuenta si la cola
esta vacia.
--------------------------------------------------------------------------------
*/

void
up(Semaphore * sem)
{
	DisableInts();
	if ( !signalQueue(sem->queue) )
		sem->value++;
	RestoreInts();
}

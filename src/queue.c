#include "../include/queue.h"

static TaskQueue time_q;

Task __taskArray[__MAX_TASKS];


void mt_initTaskQueue( TaskQueue * queue, char * name ){
	queue->name = name;
	queue->head = NULL;
	queue->tail = NULL;
	queue->iterations = 0;
}

void mt_initTaskArray( Task tasks[], int size){
	int i;
	for(i=0;i<size;i++)
		tasks[i].exists = 0;
}


Task * mt_getAvailableTask( Task tasks[], int size){
	int i;
	for(i=0;i<size;i++)
		if(tasks[i].exists == 0){
			tasks[i].exists = 1;
			return tasks+i;
		}
	return NULL;
}


/*
--------------------------------------------------------------------------------
mt_enqueue - pone un proceso a esperar en una cola de procesos

La cola esta ordenada por prioridad, siendo el ultimo proceso de la cola el
mas prioritario. Entre dos procesos de la misma prioridad, el que se inserta
ultimo queda delante del que ya estaba, de modo que si se extraen los procesos
del final de la cola se obtiene siempre el mas prioritario o el que llevaba
dormido mas tiempo entre aquellos de la misma prioridad.
--------------------------------------------------------------------------------
*/

void 
mt_enqueue(Task * task, TaskQueue * queue)
{
	Task * current;

	/* Buscar donde insertar */
	for ( current = queue->head ; current != NULL && task->priority > current->priority ; current = current->next )
		;
	if ( current != NULL ) { /* insertar antes de current */
		if ( (task->prev = current->prev) != NULL )
			current->prev->next = task;
		else
			queue->head = task;
		current->prev = task;
		task->next = current;
	}
	else if ( (current = queue->tail) != NULL){	/* insertar al final de la cola */
		queue->tail = task;
		current->next = queue->tail;
		task->prev = current;
		task->next = NULL;
	}
	else {						/* la cola esta vacia */
		queue->head = queue->tail = task;
		task->next = task->prev = NULL;
	}
	task->queue = queue;
}

/*
--------------------------------------------------------------------------------
mt_dequeue - si un proceso esta en una cola, lo desencola.
--------------------------------------------------------------------------------
*/

void 
mt_dequeue(Task * task)
{
	TaskQueue * queue;

	if ( (queue = task->queue) == NULL )
		return;

	if ( task->prev != NULL )
		task->prev->next = task->next;
	else
		queue->head = task->next;
	if ( task->next != NULL )
		task->next->prev = task->prev;
	else
		queue->tail = task->prev;
	task->next = task->prev = NULL;
	task->queue = NULL;
}

/*
--------------------------------------------------------------------------------
mt_peeklast, mt_getlast - acceso al ultimo proceso de una cola

Corresponde al proceso de mayor prioridad, o al mas viejo entre dos de la misma
prioridad. Mt_peeklast devuelve el proceso sin desencolarlo, mt_getlast lo 
desencola.
--------------------------------------------------------------------------------
*/

Task *
mt_peeklast(TaskQueue * queue)
{
	return queue->tail;
}

Task *
mt_getlast(TaskQueue * queue)
{
	Task * task;

	if ( !(task = queue->tail) )
		return NULL;
	if ( queue->tail = task->prev )
		queue->tail->next = NULL;
	else
		queue->head = NULL;
	task->prev = task->next = NULL;
	task->queue = NULL;
	return task;
}



bool	
waitQueue(TaskQueue * queue){

	bool success;

	DisableInts();
	mt_dequeue(mt_curr_task);
	mt_curr_task->state = SUSPENDED;
	mt_enqueue(mt_curr_task, queue);

	_int_20_call(0);
	success = mt_curr_task->success;
	RestoreInts();

	return success;
}


bool		
signalQueue(TaskQueue * queue)
{

	Task * task;

	DisableInts();

	if ( task = mt_getlast(queue) ){
		__ready(task, true);
		_int_20_call(0);
	}

	RestoreInts();

	return task != NULL;
}


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

*/
void			
flushQueue(TaskQueue * queue, bool success)
{
	Task * task;

	DisableInts();
	if ( mt_peeklast(queue) )
	{
		while ( (task = mt_getlast(queue)) )
			__ready(task, success);
		//scheduler();
	}
	RestoreInts();
}




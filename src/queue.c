#include "../include/queue.h"

static TaskQueue time_q;

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

/*
--------------------------------------------------------------------------------
mt_enqueue_time - pone un proceso en la cola de tiempo.

Los procesos estan ordenados por el tiempo de despertarse, en forma diferencial.
El campo ticks de cada proceso se usa para almacenar los ticks de tiempo real 
que transcurren entre el tiempo de despertarse de este proceso y del
anterior en la cola. El primer proceso a despertar es el primero de la cola, y
su campo de ticks en un momento determinado indica cuantos ticks le faltan para
despertarse. La interrupcion de tiempo real decrementa el campo de ticks del
primer proceso de esta cola. Si el campo llega a cero, despierta a este proceso
y a todos los que le sigan que tambien tengan el campo de ticks en cero.
--------------------------------------------------------------------------------
*/

void 
mt_enqueue_time(Task * task, int ticks)
{
	Task * current;

	/* Buscar donde insertar */
	for ( current = time_q.head ; current != NULL && ticks > current->ticks ; ticks -= current->ticks, current = current->time_next )
		;
	if ( current != NULL ) {		/* insertar antes de current */
		if ( (task->time_prev = current->time_prev) != NULL )
			current->time_prev->time_next = task;
		else
			time_q.head = task;
		current->time_prev = task;
		task->time_next = current;
		current->ticks -= ticks;
	}
	else if ( (current = time_q.tail) != NULL )	{ /* insertar al final de la cola */
		current->time_next = time_q.tail = task;
		task->time_prev = current;
		task->time_next = NULL;
	}
	else {						/* la cola esta vacia */
		time_q.head = time_q.tail = task;
		task->time_next = task->time_prev = NULL;
	}
	task->ticks = ticks;
	task->in_time_q = true;
}

/*
--------------------------------------------------------------------------------
mt_dequeue_time - quita un proceso de la cola de tiempo si esta en ella.
--------------------------------------------------------------------------------
*/

void 
mt_dequeue_time(Task * task)
{
	if ( !task->in_time_q )
		return;
	if ( task->time_prev != NULL )
		task->time_prev->time_next = task->time_next;
	else
		time_q.head = task->time_next;
	if ( task->time_next )
	{
		task->time_next->ticks += task->ticks;
		task->time_next->time_prev = task->time_prev;
	}
	else
		time_q.tail = task->time_prev;
	task->time_next = task->time_prev = NULL;
	task->in_time_q = false;
}

/*
--------------------------------------------------------------------------------
mt_peekfirst_time, mt_getfirst_time - acceso al primer proceso de la cola de tiempo

Corresponde al primer proceso a despertar. Mt_peekfirst_time devuelve el
proceso sin desencolarlo, mt_getfirst_time lo desencola.
--------------------------------------------------------------------------------
*/

Task *
mt_peekfirst_time(void)
{
	return time_q.head;
}

Task *
mt_getfirst_time(void)
{
	Task * task;

 	if ( (task = time_q.head) == NULL )
		return NULL;
	if ( (time_q.head = task->time_next) != NULL )
		time_q.head->time_prev = NULL;
	else
		time_q.tail = NULL;
	task->time_prev = task->time_next = NULL;
	task->in_time_q = false;
	return task;
}

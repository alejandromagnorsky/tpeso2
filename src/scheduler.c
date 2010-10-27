#include "../include/scheduler.h"
#include "../include/rand.h"

void
save_esp(int esp){
	mt_curr_task->esp = esp;
	return;
}

void __top(TaskQueue * queue ){
	int iterations = queue->iterations;
	Task * itr;
	float top = 0;
	printf("CPU Resources for queue: %s\n", queue->name);
	printf("Priority \t CPU% \t Name\n");
	for ( itr = queue->head ; itr != NULL && queue->tail ; itr = itr->next ){
		printf("%d \t\t %d \t %s\n",(int)itr->priority, (int)((100*itr->count) / ((float)iterations)), itr->name);
		top += (100*itr->count) / ((float)iterations);
	}
	printf("\t\t_______\n");
	printf("\t\t %d\n", (int)top);
}

Task * getNextTaskLottery(TaskQueue * queue){
	int qty=0;
	Task * itr;
	for(itr=queue->head; itr != NULL && queue->tail;itr=itr->next)
		qty += 4-itr->priority ;

	int ticket = (((float)_rand())/__MAXRAND)*qty;
	//printf("Qty: %d, Ticket: %d, rand: %d\n", qty, ticket, _rand());
	int i=0, tmp;
	for(itr=queue->head; itr != NULL && queue->tail;itr=itr->next)
		for(tmp=0;tmp<4-itr->priority;tmp++,i++)
			if(i == ticket){
				// Este se agrega
				itr->count++; 
				queue->iterations++;
				return itr;
			}
	return NULL;
}

Task *
getNextTask(){
	last_task = mt_curr_task;
	Task * next = getNextTaskLottery(&ready_q);
	/* Para ver el yield
	Task * next;
	if(rand()>200) 
		next = __taskQueue.head->next;
	else
		next = __taskQueue.head->next->next;*/
	if(next != NULL)
		mt_curr_task = next;
	return mt_curr_task;
}


int
load_esp(Task * task){
	return task->esp;
}

int
get_temp_esp(){
	return main_task.esp;
}


/*
--------------------------------------------------------------------------------
mt_select_task - determina el proximo proceso a ejecutar.

Retorna true si ha cambiado el proceso en ejecucion.
Llamada desde scheduler() y cuanto retorna una interrupcion de primer nivel.
Guarda y restaura el contexto del coprocesador y el contexto propio del usuario,
si existe. 
--------------------------------------------------------------------------------
*/

bool 
mt_select_task(void)
{
	Task * ready_task;

	/* Ver si el proceso actual puede conservar la CPU */
	if ( mt_curr_task->state == CURRENT )
	{
		if ( mt_curr_task->atomic_level )		/* No molestar */
			return false;

		/* Analizar prioridades y ranura de tiempo */
		ready_task = mt_peeklast(&ready_q);
		if ( !ready_task || ready_task->priority < mt_curr_task->priority ||
				ticks_to_run && ready_task->priority == mt_curr_task->priority )
			return false; 

		/* El proceso actual pierde la CPU */
		__ready(mt_curr_task, false);
	}

	/* Obtener el proximo proceso */
	last_task = mt_curr_task;
	mt_curr_task = mt_getlast(&ready_q);
	mt_curr_task->state = CURRENT;

	/* Si es el mismo de antes, no hay nada mas que hacer */
	if ( mt_curr_task == last_task )
		return false;

	/* Inicializar ranura de tiempo */
	ticks_to_run = QUANTUM;
	return true;
}


#include "../include/scheduler.h"
#include "../include/rand.h"

void
save_esp(int esp){
	mt_curr_task->esp = esp;
	return;
}

void __top(){

	DisableInts();

	TaskQueue * queue = &ready_q;

	int iterations = queue->iterations;
	Task * itr;
	float top = 0;
	printf("CPU Resources \n");
	printf("Priority\tCPU% \t Name (pid) (state) \n");
	for ( itr = queue->head ; itr != NULL && queue->tail ; itr = itr->next ){
		printf("%d \t\t %d \t %s (%d) (ready)\n", (int)itr->priority, (int)((100*itr->count) / ((float)iterations)), itr->name, itr->pid);
		top += (100*itr->count) / ((float)iterations);
	}

	queue = &blocked_q;

	for ( itr = queue->head ; itr != NULL && queue->tail ; itr = itr->next ){
		printf("%d \t\t %d \t %s (%d) (blocked)\n",(int)itr->priority, (int)((100*itr->count) / ((float)iterations)), itr->name, itr->pid);
		top += (100*itr->count) / ((float)iterations);
	}


	// Current task
	printf("%d \t\t %d \t %s (%d) (current)\n",(int)mt_curr_task->priority, (int)((100*mt_curr_task->count) / ((float)iterations)), mt_curr_task->name, mt_curr_task->pid);
	top += (100*mt_curr_task->count) / ((float)iterations);
	printf("\t\t_______\n");
	printf("\t\t %d\n", (int)top);

	RestoreInts();
}

Task * getNextTaskLottery(TaskQueue * queue){
	int qty=0;
	Task * itr;
	for(itr=queue->head; itr != NULL && queue->tail;itr=itr->next)
		qty += 4-itr->priority ;

	int ticket = (int)(((double)(((double)_rand())/((double)__MAXRAND)))*(double)qty);
//	printf("MR: %d, Qty: %d, Ticket: %d, rand: %d\n", __MAXRAND, qty, ticket, _rand());
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


	if(last_task != &main_task && last_task->state == CURRENT ){
		last_task->state = READY;
		mt_enqueue(last_task,&ready_q);
	}


	Task * next = getNextTaskLottery(&ready_q);


	if(next != NULL){
		mt_curr_task = next;
		mt_curr_task->state = CURRENT;
		mt_dequeue(mt_curr_task);
	}

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


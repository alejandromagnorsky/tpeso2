#include "../include/scheduler.h"

extern Task * vec[3];

void
save_esp(int esp){
	mt_curr_task->esp = esp;
	return;
}

Task *
getNextTask(){
	last_task = mt_curr_task;
	int n = rand();
	if(n > 200 && n < 500)
		mt_curr_task = vec[0];
	else if( n > 500)
		mt_curr_task = vec[1];
	else 
		mt_curr_task = vec[2];
	return mt_curr_task;
}


int
load_esp(Task * task){
	return task->esp;
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


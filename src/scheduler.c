#include "../include/scheduler.h"
#include "../include/rand.h"

void
save_esp(int esp){
	mt_curr_task->esp = esp;
	return;
}

int
load_esp(Task * task){

	char * vidmem = (char*) 0xb8000;
	vidmem[6] = 'L';

	return task->esp;
}

int
get_temp_esp(){
	return main_task.esp;
}


Task *
getNextTask(){

//	breakProtection();


	char * vidmem = (char*) 0xb8000;
	vidmem[4] = 'A';

	last_task = mt_curr_task;
	
	if(last_task != &main_task && last_task->state == CURRENT )
		__ready(last_task, true);

	Task * next = NULL;
	if(ticks_to_run == 0){
		next = getNextTaskRoundRobin(&ready_q);
		ticks_to_run = QUANTUM;
	}

	if(next != NULL){
		mt_curr_task = next;
		mt_curr_task->state = CURRENT;
		mt_curr_task->count++;
		mt_dequeue(mt_curr_task);
		ready_q.iterations++;
	}

	vidmem[16] = 'H';


	return mt_curr_task;
}



Task * 
getNextTaskRoundRobin(TaskQueue * queue){
	return mt_getlast(queue);
}


Task * 
getNextTaskLottery(TaskQueue * queue){
	int qty=0;
	Task * itr;
	for(itr=queue->head; itr != NULL && queue->tail;itr=itr->next)
		qty += itr->priority ;

	int ticket = (int)(((double)(((double)_rand())/((double)__MAXRAND)))*(double)qty);
//	printf("MR: %d, Qty: %d, Ticket: %d, rand: %d\n", __MAXRAND, qty, ticket, _rand());
	int i=0, tmp;
	for(itr=queue->head; itr != NULL && queue->tail;itr=itr->next)
		for(tmp=0;tmp<itr->priority;tmp++,i++)
			if(i == ticket){
				// Este se agrega				
				return itr;
			}
	return NULL;
}




void 
__top(){

	DisableInts();

	TaskQueue * queue = &ready_q;

	int iterations = queue->iterations;
	Task * itr;
	float top = 0;
	printf("CPU Resources \n");
	printf("Priority\tCPU\tPID\tName\t\t\tState\n");
	printf("--------\t---\t---\t----\t\t\t-----\n");
	for ( itr = queue->head ; itr != NULL && queue->tail ; itr = itr->next ){
		printf("%d \t\t %d \t %d \t %s \t\t\tREADY\n", (int)itr->priority, (int)((100*itr->count) / ((float)iterations)), itr->pid, itr->name);
		top += (100*itr->count) / ((float)iterations);
	}

	queue = &blocked_q;

	for ( itr = queue->head ; itr != NULL && queue->tail ; itr = itr->next ){
		printf("%d \t\t %d \t %d \t %s \t\t\tBLOCKED\n",(int)itr->priority, (int)((100*itr->count) / ((float)iterations)), itr->pid, itr->name);

		top += (100*itr->count) / ((float)iterations);
	}


	// Current task
	printf("%d \t\t %d \t %d \t %s \t\t\tCURRENT\n",(int)mt_curr_task->priority, (int)((100*mt_curr_task->count) / ((float)iterations)), mt_curr_task->pid, mt_curr_task->name);
	top += (100*mt_curr_task->count) / ((float)iterations);
	printf("\t\t  _____\n");
	printf("\t\t   %d\n", (int)top);

	RestoreInts();
}

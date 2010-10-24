#include "../include/queue.h"
#include "../include/rand.h"
#include "../include/kc.h"

void top(TaskQueue * queue, int iterations );

Task * getNextTask(TaskQueue * queue);

/*

UN EJEMPLO!


int main(){
	TaskQueue * queue = malloc(sizeof(TaskQueue));

	_srand(time(NULL));

	int i;
	for(i=0;i<10;i++){
		Task * tmp = malloc(sizeof(Task));

		float r = ((float)_rand())/__MAXRAND;
		tmp->priority = (unsigned)(r*4);
		tmp->count = 0;
		mt_enqueue(tmp, queue);
	}

	for(i=0;i<500;i++){
		getNextTask(queue);
	}
	top(queue,500);
}
*/
void top(TaskQueue * queue, int iterations ){
	int i;
	Task * itr;
	float top = 0;
	printf("Priority \t CPU% \t Name\n");
	for ( itr = queue->head ; itr != NULL && queue->tail ; itr = itr->next ){
		printf("%d \t\t %d \t Lala\n",(int)itr->priority, (int)((100*itr->count) / ((float)iterations)));
		top += (100*itr->count) / ((float)iterations);
	}
	printf("\t\t_______\n");
	printf("\t\t %d\n", (int)top);
}

Task * getNextTask(TaskQueue * queue){
	int qty=0;
	Task * itr;
	for(itr=queue->head; itr != NULL && queue->tail;itr=itr->next)
		qty += 4-itr->priority ;

	int ticket = (((float)_rand())/__MAXRAND)*qty;

	int i=0, tmp;
	for(itr=queue->head; itr != NULL && queue->tail;itr=itr->next)
		for(tmp=0;tmp<4-itr->priority;tmp++,i++)
			if(i == ticket){
				// Este se agrega
				itr->count++; 
				return itr;
			}
}





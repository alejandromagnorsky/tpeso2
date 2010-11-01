#ifndef _queue_
#define _queue_
#include "../include/mtask.h"
#include "../include/kernel.h"

#define __MAX_TASKS 500


extern Task __taskArray[__MAX_TASKS];


void mt_initTaskArray( Task tasks[], int size);
void mt_initTaskQueue( TaskQueue * queue, char * name );

void	mt_enqueue(Task * task, TaskQueue * queue);
void	mt_dequeue(Task * task);
Task *	mt_peeklast(TaskQueue * queue);
Task *	mt_getlast(TaskQueue * queue);

void	mt_enqueue_time(Task * task, int ticks);
void	mt_dequeue_time(Task * task);
Task *	mt_peekfirst_time(void);
Task * 	mt_getfirst_time(void);

void	deleteQueue(TaskQueue * queue);
bool	waitQueue(TaskQueue *queue);
bool	signalQueue(TaskQueue * queue);
void	flushQueue(TaskQueue * queue, bool success);


#endif

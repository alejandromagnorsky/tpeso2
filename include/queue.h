#ifndef _queue_
#define _queue_
#include "../include/mtask.h"

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
#endif

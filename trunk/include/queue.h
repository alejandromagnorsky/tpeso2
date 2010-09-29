#ifndef _queue_
#define _queue_
#include "../include/mtask.h"

void	mt_enqueue(Task * task, TaskQueue * queue);
void	mt_dequeue(Task * task);
Task *	mt_peeklast(TaskQueue * queue);
Task *	mt_getlast(TaskQueue * queue);

void	mt_enqueue_time(Task * task, int ticks);
void	mt_dequeue_time(Task * task);
Task *	mt_peekfirst_time(void);
Task * 	mt_getfirst_time(void);
#endif

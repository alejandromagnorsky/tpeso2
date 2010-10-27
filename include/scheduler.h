#include "../include/kernel.h"

void save_esp(int esp);
Task * getNextTask(void);
int load_esp(Task * task);
int get_temp_esp();

void __top();

Task * getNextTaskLottery(TaskQueue * queue);
Task * getNextTaskRoundRobin(TaskQueue * queue);


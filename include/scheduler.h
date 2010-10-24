#include "../include/kernel.h"

void save_esp(int esp);
Task * getNextTask(void);
int load_esp(Task * task);
bool mt_select_task(void);

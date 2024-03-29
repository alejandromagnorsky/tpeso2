#ifndef _ipc_
#define _ipc_
#include "../include/kernel.h"
#include "../include/mtask.h"
#include "../include/sem.h"
#include "../include/kc.h"

Semaphore * client_readSem;
Semaphore * server_readSem;
Semaphore * client_writeSem;
Semaphore * server_writeSem;

#define SERVER 1
#define CLIENT 0

void jokes(int argc, char * argv[]);
bool receiveMessage(int key, char * msg);
void sendMessage(int key, char * msg);

#endif

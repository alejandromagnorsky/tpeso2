#include "../include/ipc.h"

void setCRSem( Semaphore * sem){
	client_readSem = sem;
}

void setCWSem( Semaphore * sem){
	client_writeSem = sem;
}

void setSRSem( Semaphore * sem){
	server_readSem = sem;
}

void setSWSem( Semaphore * sem){
	server_writeSem = sem;
}


Semaphore * getCRSem( Semaphore * sem){
	client_readSem = sem;
}


bool
receiveMessage(int key, char * msg){
	bool success;
	if(key == SERVER)
		success = down(server_readSem);
	else
		success = down(client_readSem);
	memcpy(msg, ipcpage.shm, MAX_MSGLENGTH);
	if(key == SERVER)
		up(client_writeSem);
	else
		up(server_writeSem);
	return success;
}


void
sendMessage(int key, char * msg){
	if(key == SERVER)
		down(server_writeSem);
	else
		down(client_writeSem);
	memcpy(ipcpage.shm, msg, MAX_MSGLENGTH);
	if(key == SERVER)
		up(client_readSem);	
	else
		up(server_readSem);
}

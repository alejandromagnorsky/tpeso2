#include "../include/ipc.h"

Semaphore * client_readSem;
Semaphore * server_readSem;
Semaphore * client_writeSem;
Semaphore * server_writeSem;

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

void
jokes(int argc, char * argv[]){
	//Background
	setStdout(-1);
	setStdin(-1);
	
	char * chistes[] = {"La mayoria de los accidentes automovilisticos son culpa de los hombres... al prestarle las llaves del coche a las mujeres.",
						"Que le dijo una tanga a otra tanga? - Que conio nos ponemos hoy?",
						"El hombre soltero es un animal incompleto. El hombre casado es un completo animal.",
						"No desees la mujer de tu projimo: Date el gusto!.",
						"Flash: Super heroe de la multimedia.",
						"Tengo 2 noticias, 1 buena y otra mala. 1: he dejado las drogas. 2: no me acuerdo donde!",
						"El sexo es como jugar al poker, si no tienes un buen companiero, mas vale que tengas una buena mano.",
						"La diferencia entre un politico y un ladron es que a uno tu lo escoges y el otro te escoge a ti!",
						"Que le dijo una imagen GIF a una imagen JPEG? ¡ANIMATE!",
						"En que se parecen el aire y el sexo? En que no parecen la gran cosa, hasta que te hacen falta"};

	client_readSem = createSem("c_read", 0);
	server_readSem = createSem("s_read", 0);
	client_writeSem = createSem("c_write", 1);
	server_writeSem = createSem("s_write", 1);
	
	char msg[MAX_MSGLENGTH];
	char msg_error[MAX_MSGLENGTH] = "Error. Try with a number between 0 and 9";
	int index;
	while(true)
		if( receiveMessage(SERVER, msg) ){
			index = atoi(&msg[0]);
			if(index < 0 || index > 9)
				sendMessage(SERVER, msg_error);
			else
				sendMessage(SERVER, chistes[index]);		
		}
	
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

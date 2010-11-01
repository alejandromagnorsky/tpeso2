#include "../../include/ipc.h"

int echo(int argc, char * argv[]){
	int i;
	for(i=1;i<argc;i++)
		printf("%s ", argv[i]);
	printf("\n");
		
	// SENDER
	char msg[MAX_MSGLENGTH];
	if(argc == 2){
		sendMessage(CLIENT, argv[1]);
	}
	if(receiveMessage(CLIENT, msg))
		printf("Message received: %s\n", msg);

	return 0;
}

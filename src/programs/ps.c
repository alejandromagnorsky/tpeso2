#include "../../include/syscall.h"

int guips(int argc, char * argv[]){
	printf("PID\tTTY\tName\n");
	if( argc == 2 )
		if(!strcmp(argv[1], "-e"))
			ps(0);
		else ps(getppid());
	else ps(getppid());
} 

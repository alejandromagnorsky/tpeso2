#include "../../include/process.h"

int guips(int argc, char * argv[]){
	if( argc == 2 )
		if(!strcmp(argv[1], "-e"))
			ps(0);
		else ps(getppid());
	else ps(getppid());
} 

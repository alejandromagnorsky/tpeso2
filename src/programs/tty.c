#include "../../include/shell.h"
#include "../../include/syscall.h"

int tty(int argc, char * argv[]){

	if(argc > 0 ){

		if(!strcmp(argv[1], "-c") && argv[2] != NULL && argv[3] != NULL)
			__change_color(*argv[2] - '0',*argv[3] - '0');

		if(!strcmp(argv[1], "-s") && argv[2] != NULL)
			switchTerminal(*argv[2] - '0');

		if(!strcmp(argv[1], "-n"))
			__switch_next_terminal();

		if(!strcmp(argv[1], "-l"))
			__switch_last_terminal();

		if(!strcmp(argv[1], "-ss") && argv[2] != NULL)
			__changeSystemSymbol(argv[2]);

		return 0;
	}

	printf("Invalid arguments.\n");
	return 1;
}


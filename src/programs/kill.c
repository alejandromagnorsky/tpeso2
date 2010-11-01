#include "../../include/kc.h"
#include "../../include/syscall.h"

int guikill(int argc, char * argv[]){

	if(argc == 2){
		int pid = atoi(argv[1]);	
		if(pid == 1 )
			printf("Cannot kill Screen Manager\n");
		else if(pid == getpid())
			printf("Cannot kill self.\n");
		else if(pid != 0){
				if(kill(pid) != -1)
					printf("[%d] Killed\n", pid);
				else printf("Process not found\n");
		}else printf("Cannot kill init\n");
	} else printf("Invalid parameters \n");

}

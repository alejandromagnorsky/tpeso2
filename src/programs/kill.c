#include "../../include/process.h"
#include "../../include/kc.h"

int kill(int argc, char * argv[]){

	if(argc == 2){
		int pid = atoi(argv[1]);	
		if(pid != 0)
			_kill(pid);
		else printf("Cannot kill init\n");
	} else printf("Invalid parameters \n");

}

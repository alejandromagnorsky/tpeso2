#include "../../include/shell.h"

int man(int argc, char * argv[]){
	if(argc >= 1){
		int i, exists = 0;

		__executable * tmp = getExecutableByDescriptor(argv[1]);
		if(tmp!=NULL){
			printf("%s: %s \n", tmp->descriptor, tmp->man);
			return 0;
		} else {
			printf("Invalid argument.\n");
			return 1;
		}
	}	
	printf("Please enter a program name. \n");
	return 1;
}

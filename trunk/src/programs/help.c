#include "../../include/shell.h"

int help(int argc, char * argv[]){
	printf("Possible commands:\n");

	int i;
	for(i=0;i<__QTY_PROGRAMS;i++)
		printf("%s\n", __executable_programs[i].descriptor);
	
	return 0;
}

#include "../include/programs.h"
#include "../include/kc.h"


__executable __executable_programs[MAX_PROGRAMS];
int __QTY_PROGRAMS;

int __register_program(char * descriptor, int (*execute)(int argc, char * argv[])){
	__executable exec;
	exec.descriptor = descriptor;
	exec.execute = execute;
	exec.man = "No manual available.";

	if(__QTY_PROGRAMS < MAX_PROGRAMS){
		__executable_programs[__QTY_PROGRAMS++] = exec;
		return 1;
	}
	else return 0;
}

__executable * __getExecutableByDescriptor(char * descriptor){

	int i;
	__executable * out = NULL;
	// Search program
	for(i=0;i<__QTY_PROGRAMS;i++)
		if(! strcmp(descriptor, __executable_programs[i].descriptor))
			out = __executable_programs+i;

	return out;
}

int __register_man_page(char * descriptor, char * man){
	__executable * tmp = __getExecutableByDescriptor(descriptor);
	if( tmp == NULL )
		return 1;
	tmp->man = man;
	return 0;
}

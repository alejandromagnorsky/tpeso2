#include "../../include/syscall.h"

int clear(int argc, char * argv[]){
	clearTerminal();
	return 0;
}

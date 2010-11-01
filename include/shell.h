#ifndef _shell_
#define _shell_

#include "kc.h"

#define MAX_PROGRAMS 32
#define MAX_ARGUMENT_LENGTH 64
#define MAX_ARGUMENTS 10
#define MAX_HISTORY 20

void shell(int argc, char * argv[]);
void shellManager(int argc, char * argv[]);


#endif

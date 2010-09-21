#ifndef _shell_
#define _shell_

#include "kc.h"

#define MAX_PROGRAMS 32
#define MAX_ARGUMENT_LENGTH 64
#define MAX_ARGUMENTS 10
#define MAX_HISTORY 20

typedef struct{
	char * descriptor;				// The name
	int (*execute)(int argc, char * argv[]);	// The executable
	char * man;				// The manual page 
} __executable;

typedef struct{
	char * ptr;
	int index;
} __history_ptr;

__executable __executable_programs[MAX_PROGRAMS];
int __QTY_PROGRAMS;

// The history data
char __history[MAX_HISTORY][MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS + 1];

// History index, used when shifting history states
int __HISTORY_INDEX;

/* The history pointer stack, for pushing and iterating 
 * through history states (less time complexity than using data)
*/
__history_ptr __history_stack[MAX_HISTORY];


int __register_program(char * descriptor, int (*program)(int argc, char * argv[]));

__executable * getExecutableByDescriptor(char * descriptor);

int __register_man_page(char * descriptor, char * man);

void shell();


#endif

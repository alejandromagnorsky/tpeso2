#include "kc.h"

#define MAX_PROGRAMS 32
#define MAX_ARGUMENT_LENGTH 64
#define MAX_ARGUMENTS 10
#define MAX_HISTORY 20

typedef struct{
	char * ptr;
	int index;
} __history_ptr;

void __shift_history(int direction, char * src);

void __init_history();

void __print_history();

void __push_history_state(char * state);

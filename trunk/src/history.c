#include "../include/history.h"

// The history data
char __history[MAX_HISTORY][MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS + 1];


// History index, used when shifting history states
int __HISTORY_INDEX;

/* The history pointer stack, for pushing and iterating 
 * through history states (less time complexity than using data)
*/
__history_ptr __history_stack[MAX_HISTORY];


void __shift_history(int direction, char * src){
	int i;

	for(i=0;i<MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS;i++)
		src[i] = __history_stack[__HISTORY_INDEX].ptr[i];

	if(direction >= 0)
		if(__HISTORY_INDEX+direction<MAX_HISTORY)
			__HISTORY_INDEX+=direction;
	if(direction < 0 )
		 if(__HISTORY_INDEX+direction>=0)
			__HISTORY_INDEX+=direction;
}

void __init_history(){
	int i,j;
	for(i=0;i<MAX_HISTORY;i++){
		__history_stack[i].ptr = __history[i];
		__history_stack[i].index = i;
		for(j=0;j<MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS;j++)
			__history[i][j] = '\0';
	}
	__HISTORY_INDEX = 0;
}

void __print_history(){
	int i;
	for(i=1;i<MAX_HISTORY;i++)
		if(__history_stack[i].ptr[0] != NULL)
			printf("\t%d %s\n", i, __history_stack[i].ptr);
}

void __push_history_state(char * state){
	int i, new;

	// Restart index
	__HISTORY_INDEX = 0;
	
	// First, get index of history to delete
	new = __history_stack[MAX_HISTORY-1].index;

	// Before pushing, the stack must be shifted
	for(i=MAX_HISTORY-2;i>=0;i--){
		__history_stack[i+1].ptr = __history_stack[i].ptr;
		__history_stack[i+1].index = __history_stack[i].index;	
	}

	// Push state 
	for(i=0; i < MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS;i++ )
		__history[new][i] = state[i];
	__history_stack[0].ptr = __history[new];
	__history_stack[0].index = new;
}

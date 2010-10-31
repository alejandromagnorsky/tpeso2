#include "../include/shell.h"
#include "../include/process.h"
#include "../include/syscall.h"
#include "../include/interrupts.h"
#include "../include/programs.h"
#include "../include/console.h"
#include "../include/kc.h"

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

__executable * getExecutableByDescriptor(char * descriptor){

	int i;
	__executable * out = NULL;
	// Search program
	for(i=0;i<__QTY_PROGRAMS;i++)
		if(! strcmp(descriptor, __executable_programs[i].descriptor))
			out = __executable_programs+i;

	return out;
}

int __register_man_page(char * descriptor, char * man){
	__executable * tmp = getExecutableByDescriptor(descriptor);
	if( tmp == NULL )
		return 1;
	tmp->man = man;
	return 0;
}


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

int __moveCursorToStart(int i){
	__shift_terminal_cursor(-1,i);
	return 0;
}

int __moveCursorToEnd(char * ans, int i){
	int j = 0;	
	int tmp = i;
	while(ans[tmp] != '\0'){ j++;tmp++;}
	__shift_terminal_cursor(1,j);
	return tmp;
}

/* This is a more restricted version of scanf,
 * oriented towards shell commands, history, etc.
*/
void __getShellArguments(char * ans){
        int i = 0;
	int j;
        char c;

	// Initialize ans
        for (i=0; i < MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS ; i++)
		ans[i] = '\0';

	i = 0;
	while( (c = getchar()) != '\n' ){
		switch( c ){
			case '\b':
				if (i){
					if(ans[i] == '\0') 
						ans[--i] = '\0';
					else 
						ans[--i] = ' ';
				}
				printf("%c",c);
				break;
			

			// Terminal switch by F's
			case (char) -41: __switch_terminal(0);  printf("\n"); return;
			case (char) -42: __switch_terminal(1);	printf("\n"); return;
			case (char) -43: __switch_terminal(2);	printf("\n"); return;
			case (char) -44: __switch_terminal(3);	printf("\n"); return;
			case (char) -45: __switch_terminal(4); 	printf("\n"); return;
			case (char) -46: __switch_terminal(5); 	printf("\n"); return;
			case (char) -47: __switch_terminal(6);	printf("\n"); return;
			case (char) -48: __switch_terminal(7);	printf("\n"); return;
			case (char) -49: __switch_terminal(8);	printf("\n"); return;
			case (char) -50: printf("There is no terminal over here ;) \n"); return;

			case  (char) 204:	// RIGHT ARROW
				 if (i<MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS + 1)
					if(ans[i] != '\0'){					
						++i;
						__shift_terminal_cursor(1,1);
					}
				break;
			case (char) 202:	// UP ARROW
				for(j=0;ans[j]!='\0';j++)
					putchar('\b');

				__shift_history(1,ans);	
				printf("%s", ans);
				i = strlen(ans);
				break;
			case (char) 203:	// DOWN ARROW
				for(j=0;ans[j]!='\0';j++)
					putchar('\b');

				__shift_history(-1,ans);	
				printf("%s", ans);
				i = strlen(ans);
				break;
			case  (char) 185:	// LEFT ARROW
				if(i){
					--i;
					__shift_terminal_cursor(-1,1);
				}
				break;

			// Note: -78 and -77 are hardcoded asciis for these keys
			case  (char) -78:	// START 
				i = __moveCursorToStart(i);
				break;
			case  (char) -77:	// END
				i = __moveCursorToEnd(ans,i);
				break;
			default:
				 if (i<MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS + 1){
			                ans[i++] = c;
			                putchar(c);	
				}
				break;
		}
        }
	i = __moveCursorToEnd(ans,i);
	printf("\n");
}

int getDeadTTY(__ProcessNode * manager, int maxTTY){
	int i,j;
	for(j=0;j<maxTTY;j++){
		int exists = 0;
		for(i=0;i<__MAX_CHILDS;i++)
			if(manager->childs[i] != NULL)
				if(manager->childs[i]->data->stdinFD == j)
					exists = 1;
		if(!exists)
			return j;
	}
	return -1;
}


void init(){

	//setStdout(0);
	//setStdin(0);
/*
	char * argv[] = {"terminal-manager" , NULL };

	setPrio(3);
	int shellPID = forkexec(shellManager, 1, argv);
	setPrio(4);
	*/

	//printf("Entre!\n");
	
	// Meh
	while(1)
		wait();
}

void shellManager(int a, char * v[]){

	__getProcessNodeByPID(mt_curr_task->pid)->data->stdoutFD = 0;	

	sysinfo();
	
	// TEST ALLOCPAGE /////////////////////////////////
	unsigned int * ptr = (unsigned int *)allocPage();
	printf("1. %d\n", *ptr);
	ptr = (unsigned int *)allocPage();
	printf("2. %d\n", *ptr);
	ptr = (unsigned int *)allocPage();
	printf("3. %d\n", *ptr);
	ptr = (unsigned int *)allocPage();
	printf("4. %d\n", *ptr);
	test();
	ptr = (unsigned int *)allocPage();
	printf("5. %d\n", *ptr);
	//////////////////////////////////////////////////
	
	char * argv[] = {"terminal" , NULL };
	int shellPID;
	int i;
	for(i=0;i<10;i++){

		setStdout(i);
		setStdin(i);

		setPrio(2);
			shellPID = forkexec(shell, 1, argv);	
		setPrio(3);
	}

	while(1){
		wait();
		
		int deadTTY = getDeadTTY(__getProcessNodeByPID(mt_curr_task->pid),10);

		DisableInts();

		setStdout(__TTY_INDEX);
		printf("TTY %d killed. Calling terminal.. \n", deadTTY+1);
		RestoreInts();

		setStdout(deadTTY);
		setStdin(deadTTY);

		setPrio(2);
			shellPID = forkexec(shell, 1, argv);	
		setPrio(3);
	}
}

void shell(int a, char * v[]){

	int shelltty = __getProcessNodeByPID(mt_curr_task->pid)->data->stdoutFD;

	printf("\n");
		
	__printSystemSymbol();
	
	__init_history();


	// Data for user input
	char user_input[MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS + 1];
	int i;
	// Main loop
	while(1){

		__getShellArguments(user_input);
		if(user_input[0] != NULL)
		    __push_history_state(user_input);

		char arg_data[MAX_ARGUMENTS][MAX_ARGUMENT_LENGTH];
		char argc = 0;
		int tmp = 0;
		int background = 0;

		// Get arguments, separated by ' '
		for(i=0;user_input[i] != NULL && argc < MAX_ARGUMENTS && tmp < MAX_ARGUMENT_LENGTH;i++){
		    if(user_input[i] == ' '){          
		        if(tmp) // This way a string "     " is not considered as 6 arguments
		            arg_data[argc++][tmp] = NULL;
		        tmp = 0;
		    } else     arg_data[argc][tmp++] = user_input[i];
		}
		arg_data[argc++][tmp] = NULL;    // Last argument
	       
		if (user_input[i] != NULL)
		    printf("Error: argument too long or too much arguments.\n");
		else {
		    // Convert data to pointer
		    char * argv[MAX_ARGUMENTS] = { 0 };
		    for(i=0;i<argc;i++)
		        argv[i] = arg_data[i];

		    // Check if command is background comm
		    for(i=0;argv[0][i] != NULL;i++)
				if(argv[0][i] == '!' && argv[0][i+1] == NULL){
					argv[0][i] = NULL;
					background = 1;
				}
	   
		    __executable * exec = getExecutableByDescriptor(argv[0]);
		    if(exec != NULL){
			   if(!strcmp("exit", argv[0]))
				exit(argc,argv);				
			   else {
					if(!strcmp("waitDead",argv[0])){
						int dpid = trywait();
						printf("[ (%d) Done ]\n", dpid);
					} else {
						// Take off stdout if background process
						__getProcessNodeByPID(mt_curr_task->pid)->data->stdoutFD = background ? -1 : shelltty;	
						__getProcessNodeByPID(mt_curr_task->pid)->data->stdinFD = background ? -1 : shelltty;	
						int pid = forkexec((TaskFunc)exec->execute, argc, argv);
						// Reset stdout after fork
						__getProcessNodeByPID(mt_curr_task->pid)->data->stdoutFD = shelltty;
						__getProcessNodeByPID(mt_curr_task->pid)->data->stdinFD = shelltty;	
						if(!background)
							waitpid(pid);
						else printf("[ OK ] %d \n", pid);
						background = 0;
					}
			   }
			}
		    else if(user_input[0] != NULL)
		        printf("Error: invalid command. \n");
		}
		__printSystemSymbol();
    	}
}

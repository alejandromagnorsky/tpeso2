#include "../include/shell.h"
#include "../include/process.h"
#include "../include/syscall.h"
#include "../include/programs.h"
#include "../include/kc.h"
#include "../include/ipc.h"


int __moveCursorToStart(int i){
	shiftCursor(-1,i);
	return 0;
}

int __moveCursorToEnd(char * ans, int i){
	int j = 0;	
	int tmp = i;
	while(ans[tmp] != '\0'){ j++;tmp++;}
	shiftCursor(1,j);
	return tmp;
}

/* This is a more restricted version of scanf,
 * oriented towards shell commands, history, etc.
*/
int __getShellArguments(char * ans){
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
			case (char) -41: switchTerminal(0);return 1;
			case (char) -42: switchTerminal(1);return 1;
			case (char) -43: switchTerminal(2);return 1;
			case (char) -44: switchTerminal(3);return 1;
			case (char) -45: switchTerminal(4);return 1;
			case (char) -46: switchTerminal(5);return 1;
			case (char) -47: switchTerminal(6);return 1;
			case (char) -48: switchTerminal(7);return 1;
			case (char) -49: switchTerminal(8);return 1;
			case (char) -50: printf("There is no terminal over here ;) \n"); return;

			case  (char) 204:	// RIGHT ARROW
				 if (i<MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS + 1)
					if(ans[i] != '\0'){					
						++i;
						shiftCursor(1,1);
					}
				break;
			case (char) 202:	// UP ARROW
				for(j=0;ans[j]!='\0';j++)
					putchar('\b');

				shiftHistory(1,ans);	
				printf("%s", ans);
				i = strlen(ans);
				break;
			case (char) 203:	// DOWN ARROW
				for(j=0;ans[j]!='\0';j++)
					putchar('\b');

				shiftHistory(-1,ans);	
				printf("%s", ans);
				i = strlen(ans);
				break;
			case  (char) 185:	// LEFT ARROW
				if(i){
					--i;
					shiftCursor(-1,1);
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
	return 0;
}

void init(){
	setStdout(0);
	setStdin(0);
	char * argv[] = {"terminal-manager" , NULL };

	setPrio(3);
	int shellPID = forkexec(shellManager, 1, argv);
	argv[0] = "jokes";
	forkexec(jokes, 1, argv);
	setPrio(4);

	// Meh
	while(1)
		wait();
}

void shellManager(int a, char * v[]){

	setStdout(0);

	sysinfo();

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
		
		int deadTTY = getDeadTTY(getpid(),10);

		DisableInts();

		setStdout(getCurrentTerminal());
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

	int shelltty = getStdout();

	printf("\n");
	printSystemSymbol(shelltty+1);
	
	// Data for user input
	char user_input[MAX_ARGUMENT_LENGTH*MAX_ARGUMENTS + 1];
	int i;
	// Main loop
	while(1){

		int switched = __getShellArguments(user_input);
		if(user_input[0] != NULL)
	   	pushHistoryState(user_input);

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
	   
		    // This may cause page fault! 
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
//						setStdout(background ? -1 : shelltty);	
						setStdin(background ? -1 : shelltty);	
						int pid = forkexec((TaskFunc)exec->execute, argc, argv);
						// Reset stdout after fork
						setStdin(shelltty);
						setStdout(shelltty);
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
		if(!switched)
			printSystemSymbol(shelltty+1);
    	}
}

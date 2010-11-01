#include "../include/syscall.h"

#include "../include/kernel.h"
#include "../include/process.h"
#include "../include/programs.h"
#include "../include/console.h"
#include "../include/history.h"

/*

void __shift_terminal_cursor(int direction, int qty);

void __write_char(char c);

void __scroll_terminal();

void __changeSystemSymbol(char * str);

void __printSystemSymbol();

void __INIT_TTY();

void __clear_terminal();

int __write_terminal( const char* buffer, int count);

void __flush_terminal(int append);

int __switch_terminal(int index);

void __switch_next_terminal();




int __register_program(char * descriptor, int (*execute)(int argc, char * argv[]));
__executable * getExecutableByDescriptor(char * descriptor);
int __register_man_page(char * descriptor, char * man);



void __shift_history(int direction, char * src);

void __init_history();

void __print_history();

void __push_history_state(char * state);

void __switch_last_terminal();*/

//typedef enum { CSHIFT, HISTORYSHIFT, SWITCHTTY, PUSHHIS } ShellOp;

int int70Handler( ShellOp op, int a, void * b, int c){
	switch(op){
		case CSHIFT:
			__shift_terminal_cursor(a,c);		//Careful with hole
			break;
		case HISTORYSHIFT:
			__shift_history(a,b);
			break;
		case PUSHHIS:
			__push_history_state(b);
			break;
		case PRINTHIS:
			__print_history();
			break;
		case SWITCHTTY:
			__switch_terminal(a);
			break;
		case CLEARTTY:
			__clear_terminal();
			break;
		case GETEXE:
			return (int) __getExecutableByDescriptor(b);
			break;
		case GETTY:
			return __TTY_INDEX;
			break;
		case SYSSYM:
			__printSystemSymbol(a);
			break;
		case DEADTTY:
			return __getDeadTTY(a,c);
			break;		
	}

	return 0;
}




__executable * getExecutableByDescriptor(char * descriptor){
	return (__executable *)_int_70_call(GETEXE, 0, descriptor, 0);
}


int getDeadTTY(int managerPID, int maxTTY){
	return _int_70_call(DEADTTY, managerPID, 0,maxTTY);	
}

void printSystemSymbol(int index){
	_int_70_call(SYSSYM, index, 0, 0);	
}


void pushHistoryState(char *s){
	_int_70_call(PUSHHIS, 0, s, 0);	
}

void printHistory(){
	_int_70_call(PRINTHIS, 0, 0, 0);	
}

void shiftHistory(int dir, char * src){
	_int_70_call(HISTORYSHIFT, dir, src, 0);	
}

int getCurrentTerminal(){
	return _int_70_call(GETTY, 0, 0, 0);
}

void clearTerminal(){
	_int_70_call(CLEARTTY, 0, 0, 0);
}

void switchTerminal(int tty){
	_int_70_call(SWITCHTTY, tty, 0, 0);
}

void shiftCursor(int dir, int qty){
	_int_70_call(CSHIFT, dir, 0, qty);
}


int int80Handler( CallOp op, int a, void * b, int c){
	switch(op){
		case READ:
			return __read(a,b,c);
			break;
		case WRITE:
			return __write(a,b,c);
			break;
		case KILL:
			return __kill(a);
			break;
		case WAIT:
			__wait();
			break;
		case WAITPID:
			__waitpid(a);
			break;
		case TRYWAIT:
			return __trywait();
			break;
		case GETPID:
			return __getpid();
			break;
		case GETPPID:
			return __getppid();
			break;
		case PS:
			__ps(a);
			break;
		case FORK:
			return __forkexec((TaskFunc) a,c,b);	// Inverted!
			break;
		case PRIORITY:
			__setPriority(a);
			break;
		case SETOUT:
			__setStdout(a);
			break;
		case SETIN:
			__setStdin(a);
			break;
		case GETIN:
			return __getStdin();
			break;
		case GETOUT:
			return __getStdout();
			break;
	}

	// The cake is a lie
	return 0;
}

int kill(int pid){
	return _int_80_call(KILL, pid, 0, 0);
}

void wait(){
	_int_80_call(WAIT, 0, 0, 0);
}

void waitpid(int pid){
	_int_80_call(WAITPID, pid, 0, 0);
}

int trywait(){
	return _int_80_call(TRYWAIT, 0, 0, 0);
}

int getpid(){
	return _int_80_call(GETPID, 0, 0, 0);
}

int getppid(){
	return _int_80_call(GETPPID, 0, 0, 0);
}

int forkexec(TaskFunc f, int argc, char * argv[]){
	// CAREFUL HERE! argv inverted
	return _int_80_call(FORK, (int)f, argv, argc);
}

void ps( int pid ){
	_int_80_call(PS, pid, 0, 0);
}

void setPrio(int p){
	_int_80_call(PRIORITY, p, 0, 0);
}

void setStdout(int fd){
	_int_80_call(SETOUT, fd, 0, 0);
}

void setStdin(int fd){
	_int_80_call(SETIN, fd, 0, 0);
}

int getStdout(){
	return _int_80_call(GETOUT, 0, 0, 0);
}

int getStdin(){
	return _int_80_call(GETIN, 0, 0, 0);
}

#include "../include/syscall.h"
#include "../include/kernel.h"
#include "../include/process.h"


//typedef enum { READ, WRITE, KILL, WAIT, WAITPID, TRYWAIT, GETPID, GETPPID, PS, FORK, PRIORITY, SETOUT, SETIN } CallOp;

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
		return __forkexec((TaskFunc) a,c,b);
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




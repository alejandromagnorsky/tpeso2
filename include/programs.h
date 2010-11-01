#include "defs.h"
/***************************************************
  Programs.h
****************************************************/

#ifndef _programs_
#define _programs_


#define MAX_PROGRAMS 32
#define MAX_ARGUMENT_LENGTH 64
#define MAX_ARGUMENTS 10
#define MAX_HISTORY 20

typedef struct{
	char * descriptor;				// The name
	int (*execute)(int argc, char * argv[]);	// The executable
	char * man;				// The manual page 
} __executable;

extern __executable __executable_programs[MAX_PROGRAMS];
extern int __QTY_PROGRAMS;



int __register_program(char * descriptor, int (*execute)(int argc, char * argv[]));
__executable * __getExecutableByDescriptor(char * descriptor);
int __register_man_page(char * descriptor, char * man);



int echo(int argc, char * argv[]);
int clear(int argc, char * argv[]);
int help(int argc, char * argv[]);
int man(int argc, char * argv[]);
int call_cpuid(int argc, char * argv[]);
int gcc(int argc, char * argv[]);
int arnold(int argc, char * argv[]);
int mkexc(int argc, char * argv[]);
int time(int argc, char * argv[]);
int tty(int argc, char * argv[]);
int bingo(int argc, char * argv[]);
int reboot(int argc, char * argv[]);
int history(int argc, char * argv[]);


int daemon1(int argc, char * argv[]);

int top(int argc, char * argv[]);
int guikill(int argc, char * argv[]);
int guips(int argc, char * argv[]);
int pstree(int argc, char * argv[]);


#endif


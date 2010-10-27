#include "mtask.h"

#ifndef __PROCESS__
#define __PROCESS__

#define NULL 0

#define __PID_MAX 100
#define __MAX_CHILDS 100

typedef struct __ProcessData {
	int exists;
	// Task here
	Task * task;
} __ProcessData;

// Previous typedef for recursive declaration.
typedef struct __ProcessNode __ProcessNode;

struct __ProcessNode {
	int pid;
	__ProcessData * data;			// Process context data
	__ProcessNode * childs[__MAX_CHILDS];		// NULL terminated by convention
};


/*  Definition of process context storage.
	A process data structure is invalid
	if its "exists" flag is 0
*/
extern __ProcessData __processes[__PID_MAX];

/*  Definition of process tree storage.
	A process node is invalid if its
	data is NULL.
*/
extern __ProcessNode __processTreeData[__PID_MAX];

// The process tree.
extern __ProcessNode * __processTree;


void __initializeProcessSubSystem();

void __initializeProcessData();

void __initializeProcessTreeData();

void __initializeProcessTree();

void __addChild( __ProcessNode * parent, __ProcessNode * child);

__ProcessNode * __getProcessNodeByPID( int pid );

int __forkProcess( __ProcessNode * p);

void __killProcess( __ProcessNode * p);

void __waitProcess( __ProcessNode * parent);

void	__processCleaner( __ProcessNode * p);


void __printProcessTree( __ProcessNode * p );

void __printProcessTreeTabs( __ProcessNode * p, int tabs );


void init();

void exit(void);

void __fork();

void __exec(TaskFunc f);

// TESTEO
int __forkAndExec(TaskFunc f, char * name);





#endif

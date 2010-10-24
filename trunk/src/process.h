#ifndef __PROCESS__
#define __PROCESS__

#define NULL 0

#define __PID_MAX 1000

#define __MAX_CHILDS 1000

typedef struct __ProcessData {
	int exists;
	int test;
	// Task here
} __ProcessData;

// Previous typedef for recursive declaration.
typedef struct __ProcessNode __ProcessNode;

struct __ProcessNode {
	int pid;
	__ProcessData * data;			// Process context data
	__ProcessNode * childs[__MAX_CHILDS];		// NULL terminated by convention
};


void __initializeProcessData();

void __initializeProcessTreeData();

void __initializeProcessTree();

void __addChild( __ProcessNode * parent, __ProcessNode * child);

void __forkProcess( __ProcessNode * p);

void __killProcess( __ProcessNode * p);

void __waitProcess( __ProcessNode * parent);


void __printProcessTree( __ProcessNode * p );

void __printProcessTreeTabs( __ProcessNode * p, int tabs );





#endif

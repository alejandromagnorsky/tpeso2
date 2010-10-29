#include "../include/process.h"
#include "../include/kernel.h"
#include "../include/shell.h"
#include "../include/kasm.h"

/*  Definition of process context storage.
	A process data structure is invalid
	if its "exists" flag is 0
*/
__ProcessData __processes[__PID_MAX];

/*  Definition of process tree storage.
	A process node is invalid if its
	data is NULL.
*/
__ProcessNode __processTreeData[__PID_MAX];

// The process tree.
__ProcessNode * __processTree;


void __initializeProcessSubSystem(){
	// Initialization
	__initializeProcessData();

	__initializeProcessTreeData();

	__initializeProcessTree();
}

void __initializeProcessData(){
	__ProcessData nullData;
	nullData.exists = 0;
	nullData.stdinFD = stdin;
	nullData.stdoutFD = stdout;

	int i=0;
	for(i=0;i<__PID_MAX;i++)
		__processes[i] = nullData;
		
	return;
}

void __initializeProcessTreeData(){

	__ProcessNode nullNode;
	nullNode.data = NULL;

	int i,j;
	for(i=0;i<__PID_MAX;i++){
		__processTreeData[i].data = NULL;
		__processTreeData[i].pid = i;
		__processTreeData[i].waitpid = __WAITALL;
		for(j=0;j<__MAX_CHILDS;j++)
			__processTreeData[i].childs[j] = NULL;
	}
}

void __initializeProcessTree(){
	__processTree = __processTreeData;

	__processTree->data = __processes;
	__processTree->data->exists = 1;
	__processTree->data->task = createTask(init, (unsigned)STACKSIZE, "init", MAX_PRIO, 0);

	// Y los agrego a la cola
	mt_enqueue(__processTree->data->task, &ready_q);
}

int trywait(){
	return __tryWaitProcess(__getProcessNodeByPID(mt_curr_task->pid), __WAITALL);
}

void wait(){
	__waitProcess(__getProcessNodeByPID(mt_curr_task->pid), __WAITALL);
}

void	waitpid(int pid){
	__waitProcess(__getProcessNodeByPID(mt_curr_task->pid), pid);
}

void init(){

	printf("Init started. Calling shell...\n");

	char * argv[] = {"shell" , NULL };
	int shellPID = __forkAndExec(initShell, 1, argv);

	__ProcessNode * shellProc = __getProcessNodeByPID(shellPID);
	shellProc->data->task->priority = MAX_PRIO-1;

	while(true){
		
		wait();
		printf("Shell terminated. Calling shell...\n");


		char * argv[] = {"shell" , NULL };
		int shellPID = __forkAndExec(shell, 1, argv);

		__ProcessNode * shellProc = __getProcessNodeByPID(shellPID);
		shellProc->data->task->priority = MAX_PRIO-1;

	}
}

__ProcessNode * getAvailableProcessNode(){
	int i;
	for(i=0;i<__PID_MAX;i++)
		if(__processTreeData[i].data == NULL)
			return __processTreeData+i;
	return NULL;
}

__ProcessData * getAvailableProcessData(){
	int i;
	for(i=0;i<__PID_MAX;i++)
		if(__processes[i].exists == 0)
			return __processes+i;
	return NULL;
}

__ProcessNode * __getProcessNodeByPID( int pid ){
	return __processTree+pid;
}

void __addChild( __ProcessNode * parent, __ProcessNode * child){
	int i;
	for(i=0;i<__MAX_CHILDS;i++)
		if(parent->childs[i] == NULL){
			parent->childs[i] = child;
			return;
		}
}

void __deallocateProcess( __ProcessNode * p){
	p->data->task->exists = 0;
	p->data->exists = 0;
	p->data->stdoutFD = stdout;
	p->data->stdinFD = stdin;
	p->data = NULL;
	p->waitpid = __WAITALL;
}

int __tryWaitProcess( __ProcessNode * parent, int pid){
	int i;
	for(i=0;i<__MAX_CHILDS;i++)
		if(parent->childs[i] != NULL){
			if(parent->childs[i]->data != NULL && parent->childs[i]->data->task != NULL)
				// If child is DEAD, reap it and return
				if(parent->childs[i]->data->task->state == TERMINATED && ( pid == __WAITALL || parent->childs[i]->pid == pid )){
					__deallocateProcess(parent->childs[i]);
					int out = parent->childs[i]->pid;
					parent->childs[i] = NULL;
					return out;
				}
	}
	return -1;
}

void __waitProcess( __ProcessNode * parent, int pid){
	__tryWaitProcess(parent,pid);	

	// If no child has already died, then block until one dies
	parent->waitpid = pid;
		suspend(parent->data->task);
	parent->waitpid = __WAITALL;
	return;
}



__ProcessNode * __getParentProcessNode(__ProcessNode * p, __ProcessNode * c){

	__ProcessNode * out = NULL;

	int i;
	for(i=0;i<__MAX_CHILDS;i++)
		if(p->childs[i] != NULL){
			if(p->childs[i] == c)
				out = p;
			else out = __getParentProcessNode(p->childs[i], c);
			if(out != NULL)
				return out;
		}
	return out;
}

void __wakeParent(__ProcessNode * child){


	DisableInts();

	// At this point, child is DEAD
	__ProcessNode * init = __getProcessNodeByPID(0);

	__ProcessNode * parent = __getParentProcessNode(init, child);

	// If parent was waiting,
	if( parent != NULL && parent->data->task->state == SUSPENDED && ( parent->waitpid == __WAITALL || parent->waitpid == child->pid ) ){
		int i;
		for(i=0;i<__MAX_CHILDS;i++)
			if(parent->childs[i] == child){
				__deallocateProcess(parent->childs[i]);
				parent->childs[i] = NULL;
				__ready(parent->data->task, true);
				return;
			}
	}
	RestoreInts();
}


void __killProcess( __ProcessNode * p){

	// Dequeue task and terminate it
	// (if it is curr task, it will prevent it from being enqueued again)
	p->data->task->state = TERMINATED;
	mt_dequeue(p->data->task);

	__ProcessNode * init = __getProcessNodeByPID(0);

	int i;
	for(i=0;i<__MAX_CHILDS;i++)	
		if(p->childs[i] != NULL){
			__killProcess(p->childs[i]);
			__addChild(init, p->childs[i]);		
			p->childs[i] = NULL;
		}
}

void _kill(int pid){
	DisableInts();
		__ProcessNode * p = __getProcessNodeByPID(pid);
		printf("[%s (%d)] Killed\n", p->data->task->name, pid);

		__killProcess(p);	
		__wakeParent(p);
	RestoreInts();
}


/*
--------------------------------------------------------------------------------
exit - finaliza el proceso actual

Todos los procesos creados con CreateTask retornan a esta funcion que los mata.
Esta funcion nunca retorna.
--------------------------------------------------------------------------------
*/
void exit(int argc, char * argv[]){

	DisableInts();

	__ProcessNode * p = __getProcessNodeByPID(mt_curr_task->pid);

	__killProcess(p);	
	__wakeParent(p);

	_int_20_call(0);
	RestoreInts();
}

int __forkProcess( __ProcessNode * p){

	// Get resources
	__ProcessData * data = getAvailableProcessData();
	__ProcessNode * child = getAvailableProcessNode();

	// Copy data and activate it (should be activated anyways)
	data->exists = 1;
	data->stdinFD = p->data->stdinFD;
	data->stdoutFD = p->data->stdoutFD;
	data->task = p->data->task;
	
	// Construct child
	child->data = data;
	//child->data->task = createTask(do_nothing, (unsigned)STACKSIZE, "Child", 2, 5);
	//int aux = _copy_stack(child->data->task->stack, p->data->task->stack, STACKSIZE);
	//child->data->task->esp = aux;
	
	int i;
	for(i=0;i<__MAX_CHILDS;i++)
		if(p->childs[i] == NULL){
			p->childs[i] = child;
			return p->childs[i]->pid;
		}
	return 0;
}

void __printProcessData( __ProcessNode * p ){
		if(p->data->task->state != TERMINATED)
			printf("%s", p->data->task->name);
		else printf("Dead");
}


void __printProcessTree( __ProcessNode * p ){
	__printProcessTreeTabs(p,0);
}

void __printProcessTreeTabs( __ProcessNode * p, int tabs ){
	int i;
	for(i=0;i<tabs;i++)
		if(i)
			printf(" |\t\t");
		else printf(" \t\t");
	
	if(tabs)
		printf(" |-");

	__printProcessData(p);
	printf("(%d)", p->pid);

	int hasChilds = 0;

	for(i=0;i<__MAX_CHILDS;i++)
		if(p->childs[i] != NULL){

			// Print it nicely
			if(!hasChilds){
				hasChilds = 1;
				printf("\n");
			}

		__printProcessTreeTabs(p->childs[i], tabs+1);
	}

	if(!hasChilds) {
		printf("\n");
	}
}



// TEST !!!
int __forkAndExec(TaskFunc f, int argc, char * argv[]){

	DisableInts();

	__ProcessNode * p = __getProcessNodeByPID(mt_curr_task->pid);

	int childPID = __forkProcess(p);

	// Create child task
	__ProcessNode * child = __getProcessNodeByPID(childPID);

	//printf("STACK CHILD %d, STACK PARENT %d, ESP: %d, PRIO: %d", child->data->task->stack, p->data->task->stack, child->data->task->esp, child->data->task->priority);
	child->data->task = createTask(f, (unsigned)STACKSIZE, argv[0], p->data->task->priority, childPID);
	child->data->task->exists = 1;
	
	__ready(child->data->task, true);

	RestoreInts();
	
	return childPID;
}

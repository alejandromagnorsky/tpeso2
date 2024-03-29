#include "../include/process.h"
#include "../include/console.h"
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
	__processTree->data->task = createTask(init, 0, NULL, "init", MAX_PRIO, 0);

	// Y los agrego a la cola
	mt_enqueue(__processTree->data->task, &ready_q);
}

int __trywait(){
	return __tryWaitProcess(__getProcessNodeByPID(mt_curr_task->pid), __WAITALL);
}

void __wait(){
	__waitProcess(__getProcessNodeByPID(mt_curr_task->pid), __WAITALL);
}

void	__waitpid(int pid){
	__waitProcess(__getProcessNodeByPID(mt_curr_task->pid), pid);
}

int __getpid(){
	return mt_curr_task->pid;
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


// Reparent element!
void __addChild( __ProcessNode * parent, __ProcessNode * child){
	int i;
	for(i=0;i<__MAX_CHILDS;i++)
		if(parent->childs[i] == NULL){
			DisableInts();
				parent->childs[i] = child;
				if(child->data->task->state == TERMINATED )
					__wakeParent(child);
			RestoreInts();
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


void __setStdout(int fd){
	__getProcessNodeByPID(mt_curr_task->pid)->data->stdoutFD = fd;
}

void __setStdin(int fd){
	__getProcessNodeByPID(mt_curr_task->pid)->data->stdinFD = fd;
}


int __getStdout(){
	return __getProcessNodeByPID(mt_curr_task->pid)->data->stdoutFD;
}

int __getStdin(){
	return __getProcessNodeByPID(mt_curr_task->pid)->data->stdinFD;
}

void __waitProcess( __ProcessNode * parent, int pid){
	__tryWaitProcess(parent,pid);	

	// If no child has already died, then block until one dies
	parent->waitpid = pid;
		suspend(parent->data->task);
	parent->waitpid = __WAITALL;
	return;
}


int __getppid(){
	__ProcessNode * p = __getParentProcessNode(__getProcessNodeByPID(0),__getProcessNodeByPID(mt_curr_task->pid));
	return p != NULL ? p->pid : -1;
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
			__ProcessNode * child = p->childs[i];

			// Parent must lose child before reparenting
			p->childs[i] = NULL;
			__addChild(init, child);	
		}
}

int __kill(int pid){
	DisableInts();
		__ProcessNode * p = __getProcessNodeByPID(pid);
		if(p->data != NULL){
			__killProcess(p);	
			__wakeParent(p);
			return pid;
		}
	RestoreInts();
	return -1;
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


void __setPriority(int p){
	setPriority(mt_curr_task,p);
}

void __ps( int pid ){

	__ProcessNode * p = __getProcessNodeByPID(pid);

	printf("%d\ttty%d\t%s\n", p->pid, p->data->stdoutFD+1,p->data->task->name);

	int i;
	for(i=0;i<__MAX_CHILDS;i++)	
		if(p->childs[i] != NULL)
			__ps(p->childs[i]->pid);
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


int __getDeadTTY(int managerPID, int maxTTY){

	__ProcessNode * manager = __getProcessNodeByPID(managerPID);

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


int __forkexec(TaskFunc f, int argc, char * argv[]){

	DisableInts();

	__ProcessNode * p = __getProcessNodeByPID(mt_curr_task->pid);

	int childPID = __forkProcess(p);

	// Create child task
	__ProcessNode * child = __getProcessNodeByPID(childPID);

	//printf("STACK CHILD %d, STACK PARENT %d, ESP: %d, PRIO: %d", child->data->task->stack, p->data->task->stack, child->data->task->esp, child->data->task->priority);
	child->data->task = createTask(f, argc, argv, argv[0], p->data->task->priority, childPID);
	child->data->task->exists = 1;
	
	__ready(child->data->task, true);

	RestoreInts();
	
	return childPID;
}

#include "process.h"

#include <stdio.h>

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


void __initializeProcessData(){
	__ProcessData nullData;
	nullData.exists = 0;

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
		__processTreeData[i] = nullNode;
		__processTreeData[i].pid = i;
		for(j=0;j<__MAX_CHILDS;j++)
			__processTreeData[i].childs[j] = NULL;
	}
}

void __initializeProcessTree(){
	__processTree = __processTreeData;
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


void __killProcess( __ProcessNode * p){

	p->data->exists = 0;
	// Block task, etc.

	__ProcessNode * init = __getProcessNodeByPID(0);

	int i;
	for(i=0;i<__MAX_CHILDS;i++)	
		if(p->childs[i] != NULL){
			__killProcess(p->childs[i]);
			__addChild(init, p->childs[i]);		
			p->childs[i] = NULL;
		}

}

void __deallocateProcess( __ProcessNode * p){
	p->data = NULL;
}

void __waitProcess( __ProcessNode * parent){

	// I should block process here

	int i;
	for(i=0;i<__MAX_CHILDS;i++)
		if(parent->childs[i] != NULL)
			// If child is DEAD, reap it and return
			if(!parent->childs[i]->data->exists){
				__deallocateProcess(parent->childs[i]);
				parent->childs[i] = NULL;
				return;
			}
}

void __forkProcess( __ProcessNode * p){

	// Get resources
	__ProcessData * data = getAvailableProcessData();
	__ProcessNode * child = getAvailableProcessNode();

	// Copy data and activate it (should be activated anyways)
	*data = *(p->data);
	data->exists = 1;
	
	// Construct child
	child->data = data;

	int i;
	for(i=0;i<__MAX_CHILDS;i++)
		if(p->childs[i] == NULL){
			p->childs[i] = child;
			return;
		}
}

void __printProcessData( __ProcessNode * p ){
	if(p->data->exists)
		printf("T:%d", p->data->test);
	else printf("Dead");
}


void __printProcessTree( __ProcessNode * p ){
	__printProcessTreeTabs(p,0);
}

void __printProcessTreeTabs( __ProcessNode * p, int tabs ){
	int i;
	for(i=0;i<tabs;i++)
		if(i)
			printf("│\t");
		else printf(" \t");
	
	if(tabs)
		printf("├PID: %d ", p->pid);
	else	printf(" PID: %d ", p->pid);

	int hasChilds = 0;

	for(i=0;i<__MAX_CHILDS;i++)
		if(p->childs[i] != NULL){

			// Print it nicely
			if(!hasChilds){
				hasChilds = 1;
				printf("\u2510 ");
				__printProcessData(p);
				printf("\n");
			}

		__printProcessTreeTabs(p->childs[i], tabs+1);
	}

	if(!hasChilds) {
		__printProcessData(p);
		printf("\n");
	}
}



int main(){


	// Initialization
	__initializeProcessData();

	__initializeProcessTreeData();

	__initializeProcessTree();


	// Create main process (init)
	__processTree->data = __processes;
	__processTree->data->exists = 1;
	__processTree->data->test = 5;


	// Test forking
	__forkProcess(__processTree);

	__forkProcess(__processTree);

	__forkProcess(__processTree);

	__processTree->childs[0]->data->test = 856;

	__processTree->childs[1]->data->test = 249;



	__forkProcess(__processTree->childs[0]);

	__forkProcess(__processTree->childs[1]);

	__processTree->data->test = 376;


	__killProcess(__getProcessNodeByPID(2));

	__printProcessTree(__processTree);


	__waitProcess( __getProcessNodeByPID(0)); // Init waits

	__waitProcess( __getProcessNodeByPID(0)); // Init waits


	__printProcessTree(__processTree);


}

#include "../include/allocator.h"

#define PAGE_DIR_QTY (MEMORY_LIMIT/(PAGE_SIZE*1024))
#define EMPTY_ADDRESS 0
#define P_FLAG 1
#define RW_FLAG 2
#define U_FLAG 4
#define MAX_PROCESSES 16

char __page_status[PAGE_QTY/8];
char __active_pages[PAGE_QTY/8];

typedef struct {
	int pid;		// Process id
	void * d_page;	// Data page
	void * s_page;	// Stack page
} __Process_pages;

__Process_pages __pages_per_process[MAX_PROCESSES];

void paging();
void * map(void * phys_addr);
void * allocPage();
void protect(int pid);
__Process_pages allocProcess(int pid);

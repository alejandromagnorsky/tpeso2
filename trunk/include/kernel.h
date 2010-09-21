#include "../include/defs.h"

/********************************** 
*
*  Kernel
*
***********************************/

#ifndef _kernel_
#define _kernel_

#define OS_PID	0
#define stdout 1
#define stdin 0
#define KBUFFER_SIZE 128

typedef struct{
	int buf[KBUFFER_SIZE];
	int __KBUFFER_PTR_RD;
	int __KBUFFER_PTR_WR;
	char flag;
}__keyboard_buffer;

__keyboard_buffer kbuffer;

int (*player)(void);
int getSeconds();

typedef int size_t;
typedef short int ssize_t;
typedef enum eINT_80 {WRITE=0, READ} tINT_80;
typedef enum eUSER {U_KERNEL=0, U_NORMAL} tUSERS;

/* __write
*
* Recibe como parametros:
* - File Descriptor
* - Buffer del source
* - Cantidad
*
**/
size_t __write(int fd, const void* buffer, size_t count);

/* __read
*
* Recibe como parametros:
* - File Descriptor
* - Buffer a donde escribir
* - Cantidad
*
**/
size_t __read(int fd, void* buffer, size_t count);

#endif

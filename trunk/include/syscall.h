#include "mtask.h"

typedef enum { READ, WRITE, KILL, WAIT, WAITPID, TRYWAIT, GETPID, GETPPID, PS, FORK, PRIORITY, SETOUT, SETIN } CallOp;


int kill(int pid);

void wait();

int getpid();

int getppid();

void ps( int pid );

int trywait();

void	waitpid(int pid);

int forkexec(TaskFunc f, int argc, char * argv[]);

void setPrio(int p);

void setStdout(int fd);

void setStdin(int fd);




int	_int_80_call( CallOp op, int fd, void * buffer, int count);

int int80Handler( CallOp op, int a, void * b, int c);

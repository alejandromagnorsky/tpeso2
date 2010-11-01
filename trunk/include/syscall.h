#include "mtask.h"
#include "programs.h"

typedef enum { READ, WRITE, KILL, WAIT, WAITPID, TRYWAIT, GETPID, GETPPID, PS, FORK, PRIORITY, SETOUT, SETIN, GETOUT, GETIN } CallOp;

typedef enum { CSHIFT, SWITCHTTY, HISTORYSHIFT, PUSHHIS, PRINTHIS, CLEARTTY, GETTY, GETEXE, SYSSYM, DEADTTY } ShellOp;


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

int getStdout();

int getStdin();


void switchTerminal( int tty);

void shiftCursor(int dir, int qty);

void clearTerminal();

int getCurrentTerminal();

void printSystemSymbol(int index);

int getDeadTTY(int managerPID, int maxTTY);

__executable * getExecutableByDescriptor(char * descriptor);


void pushHistoryState(char *s);

void printHistory();

void shiftHistory(int dir, char * src);


int	_int_80_call( CallOp op, int fd, void * buffer, int count);

int int80Handler( CallOp op, int a, void * b, int c);

int	_int_70_call( ShellOp op, int fd, void * buffer, int count);

int int70Handler( ShellOp op, int a, void * b, int c);


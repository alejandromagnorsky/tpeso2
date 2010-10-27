#include "../include/rand.h"

unsigned long __Seed = __NOSEED;

#define __Multiplier ((unsigned)1103515245)
#define __Increment ((unsigned)12345)

int __randCounter = 0;
unsigned __Xn = 1;

void _srand(unsigned seed){
	__Seed = seed;
	__Xn = seed;
}

unsigned _rand(void){
	__Xn = (__Multiplier * __Xn + __Increment ) % __MAXRAND;
	return __Xn;
}

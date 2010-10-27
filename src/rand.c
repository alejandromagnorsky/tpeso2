#include "../include/rand.h"

unsigned long __Seed = __NOSEED;

#define __Multiplier 1103515245
#define __Increment 12345

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

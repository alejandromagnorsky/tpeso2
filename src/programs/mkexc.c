#include "../../include/interrupts.h"
int mkexc(int argc, char * argv[]){
	if(argc < 2 ){
		printf("Invalid quantity of arguments.\n");
		return 1;
	}
	int num = atoi(argv[1]);
	if((num == 0 && !(argv[1][0] == '0' && argv[1][1] == '\0')) 
	    || num < 0 || num > 31){
		printf("Invalid argument.\n");
		return 1;
	}		
	switch(num){
		//DIVIDE BY ZERO - LOOP
		case 0:
			int_00();
			break;
		case 1:
			int_01();
			break;
		case 2:
			int_02();
			break;
		case 3:
			int_03();
			break;
		//OVERFLOW
		case 4:
			__asm__("movb   $127, %dl\n\t"
                                "addb   $127, %dl\n\t"
                                "into");
			break;
		//BOUND CHECK
		case 5:			
			__asm__("movl	$0, -8(%ebp)\n\t"
				"movl	$2, -4(%ebp)\n\t"
				"movl	$3, %eax\n\t"
				"bound	%eax, -8(%ebp)");
			break;
		case 6:
			int_06();
			break;
		case 7:
			int_07();
			break;		
		case 8:
			int_08();
			break;
		case 10:
			int_0A();
			break;
		case 11:
			int_0B();
			break;
		case 12:
			int_0C();
			break;
		case 13:
			int_0D();
			break;
		case 14:
			int_0E();
			break;
		case 16:
			int_10();
			break;
		//RESERVED
		default : 
			int_09();
	}
	return 0;
}


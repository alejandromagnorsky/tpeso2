#include "../include/kasm.h"
#include "../include/defs.h"
#include "../include/kernel.h"
#include "../include/keyboard.h"
#include "../include/interrupts.h"

void int_00(){
	printf("Error al dividir por cero.\n");
}

void int_01(){
	printf("Debug exception.\n");
}

void int_02(){
	printf("Nonmaskable interrupt.\n");
}

void int_03(){
	printf("Breakpoint.\n");
}

void int_04(){
	printf("Overflow exception.\n");
}

void int_05(){
	printf("Bounds check.\n");
}

void int_06(){
	printf("Invalid opcode.\n");
}

void int_07(){
	printf("Coprocessor not available.\n");
}

void int_08(){
	printf("Double fault\n");
}

void int_09(){
	printf("Reserved.\n");
}

void int_0A(){
	printf("Invalid TSS.\n");
}

void int_0B(){
	printf("Segment not present.\n");
}

void int_0C(){
	printf("Stack exception.\n");
}

void int_0D(){
	printf("General protection\n");
}

void int_0E(){
	printf("Page fault.\n");
}

void int_0F(){
	printf("Reserved.\n");
}

void int_10(){
	printf("Coprocessor error.\n");
}

void int_11(){
	printf("Reserved.\n");
}

void int_12(){
	printf("Reserved.\n");
}

void int_13(){
	printf("Reserved.\n");
}

void int_14(){
	printf("Reserved.\n");
}

void int_15(){
	printf("Reserved.\n");
}

void int_16(){
	printf("Reserved.\n");
}

void int_17(){
	printf("Reserved.\n");
}

void int_18(){
	printf("Reserved.\n");
}

void int_19(){
	printf("Reserved.\n");
}

void int_1A(){
	printf("Reserved.\n");
}

void int_1B(){
	printf("Reserved.\n");
}

void int_1C(){
	printf("Reserved.\n");
}

void int_1D(){
	printf("Reserved.\n");
}

void int_1E(){
	printf("Reserved.\n");
}

void int_1F(){
	printf("Reserved.\n");
}

void int_20() {

}

void int_21(){
	int scanCode = _read_scancode();
	int asciiCode = getAscii(scanCode);
	
	if(asciiCode != -1){
		kbuffer.buf[kbuffer.__KBUFFER_PTR_WR] = asciiCode;
		kbuffer.__KBUFFER_PTR_WR = (kbuffer.__KBUFFER_PTR_WR+1) % KBUFFER_SIZE;	
	}
}


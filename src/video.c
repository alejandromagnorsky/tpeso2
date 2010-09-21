#include "../include/kasm.h"
#include "../include/video.h"


int __register_special_ascii(int ascii, char repr){
	__special_ASCII newAscii;
	newAscii.ascii = ascii;
	newAscii.repr = repr;
	if(__QTY_SPECIAL_ASCII < MAX_SPECIAL_ASCII){
		__special_ASCII_map[__QTY_SPECIAL_ASCII++] = newAscii;
		return 1;
	} else return 0;
}


int __write_screen(char c, int pos, int attr){

	if( pos > 80*25 || pos < 0)
		return -1;

	char * vidmem = (char*) 0xb8000;
	int i;
	for(i=0;i<MAX_SPECIAL_ASCII;i++)
		if( __special_ASCII_map[i].ascii == c)	
			c = __special_ASCII_map[i].repr;

	vidmem[pos*2] = c;
	vidmem[(pos*2)+1] = attr;

	return pos;
}


void __flush_screen(char * screen, int from, int to, int attr){
	_turn_cursor_off();
	int i;
	for(i=from;i<=to;i++)
		__write_screen(screen[i], i,attr);
	_move_cursor(to);
	_turn_cursor_on();
}

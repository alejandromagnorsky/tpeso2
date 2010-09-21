#ifndef _video_
#define _video_

#define MAX_SPECIAL_ASCII 5

typedef struct{
	int ascii;
	char repr;
} __special_ASCII;

__special_ASCII __special_ASCII_map[MAX_SPECIAL_ASCII];
int __QTY_SPECIAL_ASCII = 0;



int __register_special_ascii(int ascii, char repr);

int __write_screen(char c, int pos, int attr);

void __flush_screen(char * screen, int from, int to, int attr);

#endif

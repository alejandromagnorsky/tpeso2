#ifndef _terminal_
#define _terminal_



#define WHITE_FG 0x07 
#define ORANGE_FG 0x06
#define VIOLET_FG 0x05
#define RED_FG 0x04
#define SKY_FG 0x03
#define GREEN_FG 0x02
#define BLUE_FG 0x01
#define BLACK_FG 0x00

#define WHITE_BG 0x70 
#define ORANGE_BG 0x60
#define VIOLET_BG 0x50
#define RED_BG 0x40
#define SKY_BG 0x30
#define GREEN_BG 0x20
#define BLUE_BG 0x10
#define BLACK_BG 0x00

#define __MAX_TERMINALS 10
#define __TAB_LENGTH 5
#define __MAX_SS 15

#define __BLOCK_ASCII -10
#define __TAB_ASCII -11
#define __ENTER_ASCII -12


typedef struct{
	char buf[80*25];
	int ptr;
	int attr;
	int empty;
} __terminal;


// Console buffers
__terminal __tty[__MAX_TERMINALS];

// Index of visible console
int __TTY_INDEX = 0;

char __SYSTEM_SYMBOL[__MAX_SS] = "tty";


void __shift_terminal_cursor(int direction, int qty);

void __write_char(char c);

void __scroll_terminal();

void __changeSystemSymbol(char * str);

void __printSystemSymbol();

void __INIT_TTY();

void __clear_terminal();

int __write_terminal( const char* buffer, int count);

void __flush_terminal(int append);

int __switch_terminal(int index);

void __switch_next_terminal();

void __switch_last_terminal();

#endif


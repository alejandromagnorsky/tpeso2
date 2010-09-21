#include "../include/console.h"
#include "../include/shell.h"

void __INIT_TTY(){

	__register_special_ascii(__BLOCK_ASCII, ' ');
	__register_special_ascii(__TAB_ASCII, ' ');
	__register_special_ascii(__ENTER_ASCII, ' ');

	int i=0,j=0;

	// Initialize all consoles
	for(;i<__MAX_TERMINALS;i++){
		__tty[i].ptr = 0;
		__tty[i].attr = GREEN_FG | BLACK_BG;
		__tty[i].empty = 1;
	
		while(j<80*25)
			__tty[i].buf[j++] = ' ';
	}

	__TTY_INDEX = 0;
	__flush_terminal(0);
}

void __change_color(int fg, int bg){

	int colorFG, colorBG = 0;
	switch(fg){
		case 0:	colorFG=BLACK_FG; break;
		case 1:	colorFG=WHITE_FG; break;
		case 2:	colorFG=ORANGE_FG; break;
		case 3:	colorFG=VIOLET_FG; break;
		case 4:	colorFG=RED_FG; break;
		case 5:	colorFG=SKY_FG; break;
		case 6:	colorFG=GREEN_FG; break;
		case 7:	colorFG=BLUE_FG; break;
		default: colorFG=WHITE_FG;break;
	}

	switch(bg){
		case 0:	colorBG=BLACK_BG; break;
		case 1:	colorBG=WHITE_BG; break;
		case 2:	colorBG=ORANGE_BG; break;
		case 3:	colorBG=VIOLET_BG; break;
		case 4:	colorBG=RED_BG; break;
		case 5:	colorBG=SKY_BG; break;
		case 6:	colorBG=GREEN_BG; break;
		case 7:	colorBG=BLUE_BG; break;
		default: colorBG=BLACK_BG;break;
	}

	__tty[__TTY_INDEX].attr = colorBG | colorFG;

	// To flush or not to flush? That is the question.
	__flush_terminal(0);
}

void __clear_terminal() {
	int i = 0;
	while( i< (80*25))
		__tty[__TTY_INDEX].buf[i++] = ' ';
	__tty[__TTY_INDEX].ptr = 0;
	__tty[__TTY_INDEX].empty = 1;

	__flush_terminal(0);
}

void __changeSystemSymbol(char * str){
	int i;
	for(i=0;str[i] != '\0';i++)
		__SYSTEM_SYMBOL[i] = str[i];
	__SYSTEM_SYMBOL[i] = '\0';
}

void __printSystemSymbol(){
	printf("%s/%d/:%c", __SYSTEM_SYMBOL, __TTY_INDEX+1, __BLOCK_ASCII);
}

void __enter(){
	__terminal * act_tty = __tty + __TTY_INDEX;
	act_tty->ptr++; // Start on the next character
	int row = (act_tty->ptr/80) % 25;
	int append = 0;

	// Maybe last character was last in row
	if( 80*(row+1) - act_tty->ptr  <= 1 ) 
		return;

	// Fill with special ascii's 
	while(act_tty->ptr<80*(row+1)){
		act_tty->buf[act_tty->ptr++] = __ENTER_ASCII;
		append++;
	}

	// Check if scroll needed, and flush
	if(row == 24 )
		__scroll_terminal();
	__flush_screen(act_tty->buf,act_tty->ptr-append, act_tty->ptr, act_tty->attr);
}

void __tab(){
	int i;
	__terminal * act_tty = __tty + __TTY_INDEX;
	for(i=0;i<__TAB_LENGTH;i++){
		if(act_tty->ptr == 80*25 - 1 )
			__scroll_terminal();
		act_tty->buf[act_tty->ptr++] = ' ';	
	}
	act_tty->buf[act_tty->ptr-1] = __TAB_ASCII;
}

void __backspace(){
	__terminal * act_tty = __tty + __TTY_INDEX;
	char c = act_tty->buf[act_tty->ptr-1];
	int append = 0;
	switch(c){
	case __BLOCK_ASCII:
		break;
	case __TAB_ASCII:
		for(;append<__TAB_LENGTH;append++)
			act_tty->buf[--act_tty->ptr] = ' ';
		break;
	case __ENTER_ASCII:
		for(;act_tty->buf[act_tty->ptr-1] == __ENTER_ASCII;append++)
			act_tty->buf[--act_tty->ptr] = ' ';
		break;
	default:
		act_tty->buf[--act_tty->ptr] = ' ';		
	}

	__flush_screen(act_tty->buf,act_tty->ptr, act_tty->ptr+append, act_tty->attr);
	return;
}

void __shift_terminal_cursor(int direction, int qty){
	__terminal * act_tty = __tty + __TTY_INDEX;
	int i = 0;
	if(qty == 0 || direction == 0 ) return;

	while(abs(i) != qty ){
		char c = act_tty->buf[act_tty->ptr+direction]; // get next character
		switch(c){
			case __BLOCK_ASCII:
				// If blocked, just flush to set cursor and return
				__flush_screen(act_tty->buf,act_tty->ptr, act_tty->ptr, act_tty->attr);
				return;
				break;
			default:
				act_tty->ptr += direction;		
		}
		i+=direction;
	}

	// Null flush, just to move cursor over there
	__flush_screen(act_tty->buf,act_tty->ptr, act_tty->ptr, act_tty->attr);
	return;
}

void __write_char(char c){
	__terminal * act_tty = __tty + __TTY_INDEX;
	act_tty->buf[act_tty->ptr++] = c;	

	if(act_tty->ptr == 80*25 )
		__scroll_terminal();
}

int __write_terminal( const char* buffer, int count){

	int i = 0;
	int j;
	int append = count;

	// The pointer to the active terminal
	__terminal * act_tty = __tty + __TTY_INDEX;
	act_tty->empty = 0;

	while(i<count){

		// THIS SHOULD NEVER HAPPEN. But the pointer is public, so it can.
		if(act_tty->ptr >= 80*25 || act_tty->ptr < 0)
			act_tty->ptr = 0;

		char c = buffer[i];

		// This behavior is the same for the shell and for scanf
		switch( c ){
			case '\n':
				__enter();	// ENTER
				break;
			case '\t':
				__tab();	// TAB
				break;
			case '\b':
				__backspace();	// BACKSPACE
				break;
			default:
				__write_char(c);
				break;
		}
		i++;
	}

	__flush_terminal(append);

	return count;
}

void __scroll_terminal(){

	int i=0;
	for(;i<80*24;i++)
		__tty[__TTY_INDEX].buf[i] = __tty[__TTY_INDEX].buf[i+80];
	for(i=0;i<80;i++)
		__tty[__TTY_INDEX].buf[80*24+i]= ' ';
	__tty[__TTY_INDEX].ptr = 80*24;
	__flush_terminal(0);
}

void __flush_terminal(int append){
	// The pointer to the active terminal
	__terminal * act_tty = __tty + __TTY_INDEX;
	
	if(append) 
		__flush_screen(act_tty->buf, act_tty->ptr-append, act_tty->ptr, act_tty->attr);
	 else
		__flush_screen(act_tty->buf,0, 80*25,act_tty->attr);

	return;
}

void __switch_next_terminal(){
	if(++__TTY_INDEX == __MAX_TERMINALS)
		__TTY_INDEX = 0;
	__flush_terminal(0);
}

void __switch_last_terminal(){
	if(--__TTY_INDEX < 0)
		__TTY_INDEX = __MAX_TERMINALS-1;
	__flush_terminal(0);
}

int __switch_terminal(int index){
	if(index >= __MAX_TERMINALS )
		return -1;
	else {
		__TTY_INDEX = index;
		__flush_terminal(0);
	}
	
	return index;
}


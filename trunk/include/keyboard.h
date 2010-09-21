#ifndef _keyboard_
#define _keyboard_

/* Recives an scancode and return his ASCII value
** If is a break code, returns -1
*/
int getAscii(int scanCode);

/*
** Returns 1 if the scanCode is a letter (includes ñ) or 0 otherwise
*/
char isLetter(int scanCode);

/*
** Returns 1 if the scanCode is a number or a ".", or 0 otherwise
*/
char isNumber(int scanCode);

/*
** Returns 1 if the scanCode is a vocal
*/
char isVocal(int scanCode);

/*
** Returns the vocal with tilde
*/
char getTildeVocal(int scanCode);

/*
** Manipulate keyboards LEDs (Caps, Num-Block, Scroll-block)
*/
void keyboardLeds();

/*
** User-welcome fireworks
*/
void fireWorks();
#endif

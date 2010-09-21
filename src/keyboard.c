#include "../include/keyboard.h"
#include "../include/kasm.h"

/* LED variables */
int __CURRENT_LEDS = 0;
int __SCROLL_LED = 1;
int __NUM_LED = 2;
int __CAPS_LED = 4;

/* Special ASCII flags [0 = disabled] */
int __L_SHIFT_ = 0;
int __R_SHIFT_ = 0;
int __CAPSLOCK_ = 0;
int __NUMLOCK_ = 0;
int __TILDE_ = 0;
int __EXTENDED_ = 0;


int makeCodes[83] =	 {-1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 39, 168, '\b', '\t', 'q', 'w', 'e', 'r', 
			  't', 'y', 'u', 'i', 'o', 'p', '{', '+', '\n', -1, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 
                          164, -5, '}', 124, -5, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', -1, '*', -1, ' ', -5,
			  -41, -42, -43, -44, -45, -46, -47, -48, -49, -50, 'N', 'S', 'H', 202, -1, '-', 185, 206, 204, '+', 'E',
			  203, -1, 'I', 'D'};

int shiftMakeCodes[83] = {-1, '!', '"', '#', '$', '%', '&', '/', '(', ')', '=', '\?', 173, '\b', '\t', 'Q', 'W', 'E', 'R', 
			  'T', 'Y', 'U', 'I', 'O', 'P', '[', '*', '\n', -1, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 
			  165, -5, ']', 167, -5, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', -5, '*', -1, ' ', -5,
			  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 'N', 'S', 'H', 202, -1, '-', 185, 206, 204, '+', 'E',
			  203, -1, 'I', 'D'};

int numpadMakeCodes[13] = {'7', '8', '9', -1, '4', '5', '6', -1, '1', '2', '3', '0', '.'};

int extendedMakeCodes[83] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			     -1, -1, -1, -1, '\n', -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
			     -1, -1, -1, -1, -1, -1, -1, '/', -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
			     -1, -1, -1, -78, 202, -1, '-', 185, 206, 204, -1, -77, 203, -1, 'I', 'D'};

int getAscii(int scanCode){
	int asciiCode = -1;
	scanCode = scanCode & 0xFF;

	//For scanCodes with more than 1 byte
	if(__EXTENDED_ != 0){
		__EXTENDED_ = 0;
		if(scanCode <= 83)
			asciiCode = extendedMakeCodes[scanCode-1];
	} else if(scanCode == 0xE0) 
		__EXTENDED_ = 1;

        else if (scanCode == 170)
                __L_SHIFT_ = 0;  
        else if(scanCode == 182)
                __R_SHIFT_ = 0;
        // If the scanCode is a make code
        else if (scanCode <= 128){
                if(__TILDE_){
                        if(isVocal(scanCode))
                                asciiCode = getTildeVocal(scanCode);
                        __TILDE_ = 0;
                } else if(scanCode == 40){
                        __TILDE_ = 1;
                } else if(scanCode == 42)
                        __L_SHIFT_ = 1;
                else if(scanCode == 54)
                        __R_SHIFT_ = 1;
                else if (scanCode == 58){
                        __CURRENT_LEDS += __CAPS_LED;
                        keyboardLeds(__CURRENT_LEDS);
                        __CAPS_LED *= -1;
                        __CAPSLOCK_ = !__CAPSLOCK_;
                }
                else if(scanCode == 69){
                        __CURRENT_LEDS += __NUM_LED;
                        keyboardLeds(__CURRENT_LEDS);
                        __NUM_LED *= -1;
                        __NUMLOCK_ = !__NUMLOCK_;               
                }else if (__CAPSLOCK_ && isLetter(scanCode) ){
                        if (!__L_SHIFT_ && !__R_SHIFT_)
                                asciiCode = shiftMakeCodes[scanCode-1];
                        else
                                asciiCode = makeCodes[scanCode-1];
                }else if( (__L_SHIFT_ || __R_SHIFT_ || __NUMLOCK_) && isNumber(scanCode))
                        asciiCode = numpadMakeCodes[scanCode-71];
                else if(__L_SHIFT_ || __R_SHIFT_)
                        asciiCode = shiftMakeCodes[scanCode-1];              
                else
                        asciiCode = makeCodes[scanCode-1];
        }
        return asciiCode;
}

char isLetter(int scanCode){
        return (makeCodes[scanCode-1] <= 'z' && makeCodes[scanCode-1] >= 'a') 
                 || makeCodes[scanCode-1] == 164;
}

char isNumber(int scanCode){
        return (scanCode >= 71 && scanCode <= 73) 
                 || (scanCode >= 75 && scanCode <= 77)
                 || (scanCode >= 79 && scanCode <= 83);
}

char isVocal(int scanCode){
        return (makeCodes[scanCode-1] == 'e' || makeCodes[scanCode-1] == 'u' || 
                makeCodes[scanCode-1] == 'i' || makeCodes[scanCode-1] == 'o' 
                || makeCodes[scanCode-1] == 'a');
}

char getTildeVocal(int scanCode){
        int ans;
        switch(makeCodes[scanCode-1]){
                case 'e':
                        ans = 130;
                        break;
                case 'u':
                        ans = 163;
                        break;
                case 'i':
                        ans = 161;
                        break;
                case 'o':
                        ans = 162;
                        break;
                case 'a':
                        ans = 160;
        }
        if(__CAPSLOCK_ || __L_SHIFT_ || __R_SHIFT_)
                ans = -1;
        return ans;
}

void keyboardLeds(int leds){
	/* Read until keyboard buffer if empty (bit 1 in port 64h must be zero). */
        while ( (_inport(0x64) & 2) != 0);
        _outport(0x60, 0xED);   /* Send commando code: 0xED -> Next byte sent to port 60h will change LEDs status */

	/* Empty buffer again */
        while ( (_inport(0x64) & 2) != 0);
        _outport(0x60, leds);	/* Send LEDs status byte */
}

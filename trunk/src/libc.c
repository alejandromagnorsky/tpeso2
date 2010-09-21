#include "../include/kernel.h"
#include "../include/shell.h"
#include "../include/kc.h"

#define MAX_USER_LENGTH MAX_ARGUMENTS*MAX_ARGUMENT_LENGTH

int getchar(){
        return getc(stdin);
}

int getc(int fd){
        int character = -1;
        while (character == -1)
                read(fd, &character, 1);
        return character;
       
}

int abs(int x){
	return x < 0 ? -x : x;
}


int atoi(const char *str){
        int i, acum, flagNegative = 0;
        for (i = 0, acum = 0; (!i && str[i]=='-') || isDigit(str[i]) && str[i] != '\0'; i++){
                if (str[i]=='-')
                        flagNegative = 1;
                else
                        acum = acum * 10 + str[i] - '0';
        }
        if (i==strlen(str))
                return flagNegative * -1 * acum + !flagNegative * acum;
        return 0;
}


/*
** NOTE:
** 	This implementation of 'getString' is aimed for being used only by 'scanf' function (where
** 'ans' is always a vector of MAX_STRLEN chars), as i counter is only compared against MAX_STRLEN
** constant (in the 'else if' clause), regardless the length of 'ans'.
*/
void getString(char * ans){
        int i, j;
        char c;
        for (i=0, j=0; (c = getchar()) != '\n' ; j++){
                printf("%c", c);
                if(c == '\b'){
                        if (i)          
                                ans[--i] = '\0';
                } else if (i<MAX_USER_LENGTH){
                        ans[i] = c;
                        i++;
                }
        }
        ans[i] = '\0';
}


/*
** NOTE:
**	As we can't make use of 'malloc' function, we decided to set MAX_USER_LENGTH constant with the
** max quantity of characters the user can enter. Then, 'scanf' reserves MAX_USER_LENGTH+1 bytes for
** vector strIn (strIn[MAX_USER_LENGTH] = '\0').
*/

int scanf(const char * str, ...){
        int i, j, k, acum, strLen, strTrueLen, strInLen, flagNegative;
        /*      strLen:         'str' length. Scanf's string.
        **      strTrueLen:     final 'str' length, once every %x in 'str' has been replaced with values.
        **      strInLen:       'strIn' length, user-entered string. 
        */
        char strIn[MAX_USER_LENGTH+1];	/* Last position is reserved for '\0'; user can effectively enter MAX_USER_LENGTH chars */
        void ** argv = (void **)(&str);
        getString(strIn); /* User enters a string */
        strLen = strlen(str);
        strTrueLen = strLen;    /* At start, boths lengths are equals, because no %x has been replaced */
        strInLen = strlen(strIn);

        for (i=0, j=0 ; i<strLen && j<strInLen; i++, j++){
                if ( i+1 != strLen && str[i] == '%' )
                        switch(str[++i]){
                                case 'd':
                                        for(k=0, acum = 0, flagNegative = 0; ( strIn[j] == '-' && !k ) || isDigit(strIn[j])  ; k++, j++){
                                                if (strIn[j] != '-'){
                                                        acum = acum * 10 + strIn[j] -'0';
                                                } else
                                                        flagNegative = 1;
                                        }
                                        j--;
                                        if (!k){
                                                return -1;
                                        } else if (!isDigit(strIn[j]) && flagNegative){
                                                return -1;
                                        }
                                        *(*((int **)++argv)) = flagNegative * -1 * acum + !flagNegative * acum;
                                        strTrueLen = strTrueLen - 2 + k;  /* Sub 2 ('%' and 'd') and add k (qty. of read digits - 1) */
                                        break;
                                case 'c':
                                        *(*((char **)++argv)) = strIn[j];
                                        strTrueLen--;
                                        break;
                                case 's':
                                        argv++;
					/* NOTE:
					** 	i+1 can be equal to strLen as 'str' is a null-terminated const char *.
					** So, str[strLen] = '\0';
					*/
					if ( i+1 <= strLen){
                                   	     for (k=0; k<MAX_USER_LENGTH && strIn[j] != str[i+1] ; k++, j++)
                                  	              ((char *)(*((char **) argv)))[k] = strIn[j];
                                    	    ((char *)(*((char **) argv)))[k] = '\0';
                                        }
					j--;
                                        strTrueLen = strTrueLen -2 + k;
                                        break;

				/* NOTE:
				** 	'default' case works this way to keep analogy with 'printf' function. If it detects a '%' and next character
				** is neither 'd' nor 'c' nor 's', then it skips both characters ('%' and the next one). 
				*/
                                default:
                                        j--;
                                        strTrueLen-=2;
                                        break;
                        }
                else
                      if (str[i] != strIn[j])
                                return -1;
        }
        if (strInLen != strTrueLen)
                return -1;
        printf("\n");   /* The user entered '\n', so it must be used */
        return 0;
}


int read(int fd, int * c, size_t count){
        return _int_80_hand(0, fd, c, count);
}


int strlen(const char * str){
        int i;
        for (i = 0; str[i] != '\0'; i++);
                return i;
}

int isDigit(int a){
        if( (a >= '0') && (a <= '9') )
                return 1;
        return 0;
}

int putchar(int c){
        return putc(stdout, c);
}

int putc(int fd, int c){
        return write(fd, &c, 1);
}

int printf(const char * str, ...){
        int c;
        int wait = 0;
        void ** argv = (void **)(&str);

        for(c=0;str[c] != NULL;c++){
                if(wait){
                        switch(str[c]){
                        case 'd':
                                putInt( *( (int*) argv));
                                break;
                        case 'c':
                                putchar(*((char*)argv));
                                break;
                        case 's':
                                printf(*((char**)argv));
                                break;
                        default:
                                break;
                        }
                        wait = 0;
                } else {
                        if(str[c] == '%'){
                                wait = 1;      
                                argv++;
                        }
                        else
                                putchar(str[c]);
                }
        }
}

int write(int fd, int * c, size_t count){
        return _int_80_hand(1, fd, c, count);
}

int pow(int n, int exp){
        int i;
        int out = 1;
        for(i=0;i<exp;i++)
                out*=n;
        return out;
}

void putInt(int n){

        int i,j;
       
        if(n<0){
                putchar('-');
                n = -n;
        }


        for(i=0;n/pow(10,i) > 0 ;i++); /* char quantity */
        for(j=i-1;j>0;j--){
                putchar( (n/pow(10,j))+'0');
                n = n%pow(10,j);
        }
        putchar((n%10)+'0');
}

int strcmp(const char * str1, const char * str2){

        int i = 0;
        int out = 0;
        while(str1[i] != NULL || str2[i] != NULL){

                if( str1[i] != str2[i])
                        return str1[i] - str2[i];

                i++;
        }

        if(str1[i] == NULL && str2[i] != NULL)
                return -str2[i];
        else if( str1[i] != NULL && str2[i] == NULL)
                return str1[i];

        return out;
}

int rand(){
        int seconds = getSeconds();
	int acum1, acum2;

	/* NOTE 1:
	**	- Max value an integer can handle is 2^32. So 'seconds%32' makes sure pow's return value fits an integer
	**	- '(int)(seconds/32)' is added to avoid cases like:
	**			pow(2,59) = pow(2,27)		--> acum1 would be the same for seconds=n and seconds=n-32
	**		Thanks to the addition:
	**			pow(2,59) + 1 != pow(2,27) + 0	--> acum1 can be assigned 60 different values
	*/
	acum1 = pow(2, seconds%32) + (int)(seconds/32);

	/* NOTE 2:
	**	- Max value an integer can handle is 2^32. But 3^20 < 2^32 < 3^21. So 'seconds%20' makes sure pow's return value fits an integer.
	**	- '(int)(seconds/20)' is added to avoid cases similar to the ones in *NOTE 1*
	*/
	acum2 = pow(3, seconds%20) + (int)(seconds/20);

        return acum2%acum1;
}

void wait(int seconds){
	int i, lastSec = -1;
	int actSec = getSeconds();

	for (i=0; i <= seconds; i++){
		lastSec = actSec;
		actSec = getSeconds();
		if (lastSec == actSec)
			i--;
	}
}

/***************************************************************
*setup_IDT_entry
* Inicializa un descriptor de la IDT
*
*Recibe: Puntero a elemento de la IDT
*        Selector a cargar en el descriptor de interrupcion
*        Puntero a rutina de atencion de interrupcion  
*        Derechos de acceso del segmento
*        Cero
****************************************************************/

void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access,
                         byte cero) {
  item->selector = selector;
  item->offset_l = offset & 0xFFFF;
  item->offset_h = offset >> 16;
  item->access = access;
  item->cero = cero;
}


int sysinfo(){
	printf("\n                                                                     2 0 1 0  \n");   
	printf("\n  __    __   ______   __               ______    ______    ______    ______  \n");
	printf("  |  \\  |  \\ /      \\ |  \\             /      \\  /      \\  /      \\  /      \\ \n");
	printf("  | $$  | $$|  $$$$$$\\| $$            |  $$$$$$\\|  $$$$$$\\|  $$$$$$\\|  $$$$$$\n");
	printf("  | $$__| $$| $$__| $$| $$            | $$__/ $$| $$$\\| $$| $$$\\| $$| $$$\\| $$\n");
	printf("  | $$    $$| $$    $$| $$             \\$$    $$| $$$$\\ $$| $$$$\\ $$| $$$$\\ $$\n");
	printf("  | $$$$$$$$| $$$$$$$$| $$             _\\$$$$$$$| $$\\$$\\$$| $$\\$$\\$$| $$\\$$\\$$\n");
	printf("  | $$  | $$| $$  | $$| $$_____       |  \\__/ $$| $$_\\$$$$| $$_\\$$$$| $$_\\$$$$\n");
	printf("  | $$  | $$| $$  | $$| $$     \\       \\$$    $$ \\$$  \\$$$ \\$$  \\$$$ \\$$  \\$$$\n");
	printf("   \\$$   \\$$ \\$$   \\$$ \\$$$$$$$$        \\$$$$$$   \\$$$$$$   \\$$$$$$   \\$$$$$$ \n");
	printf("  ____________________________________________________________________________\n");
	printf("     Time                                                       Hello, Dave\n   ");
	time(1, -1);
	printf("\n                        'help' para mas informacion \n");
	printf("\n ============================================================================\n");
}


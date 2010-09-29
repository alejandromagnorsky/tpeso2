/**********************
 kc.h
**********************/
#include "defs.h"
#include "kernel.h"

#ifndef _kc_
#define _kc_

#define NULL 0

int abs(int x);

/* Muestra la imagen de inicio */
void showSplashScreen();

int putchar(int character);

int atoi(const char *str);

/* Imprime un caracter en pantalla */
int putc(int fd, int character);

/* Imprime un string en pantalla */
int printf(const char * str,...);

int write(int fd, int * c, size_t count);

int getchar();

int getc(int fd);

int scanf(const char * str, ...);

int read(int fd, int * c, size_t count);

/* Compara dos strings */
int strcmp(const char * str1, const char * str2);

/* Imprime un numero */
void putInt(int n);

int rand();

/* Tiempo de espera */
void wait(int time);

/* Inicializa la entrada del IDT */
void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access,
			 byte cero);

#endif

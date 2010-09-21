/*********************************************
kasm.h

************************************************/

#include "defs.h"
#include "kernel.h"

#ifndef _kasm_
#define _kasm_
unsigned int    _read_msw();

void            _lidt (IDTR *idtr);

void		_mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void		_mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void		_Cli(void);        /* Deshabilita interrupciones  */
void		_Sti(void);	 /* Habilita interrupciones  */

void		_int_00_hand();
void		_int_01_hand();
void		_int_02_hand();
void		_int_03_hand();
void		_int_04_hand();
void		_int_05_hand();
void		_int_06_hand();
void		_int_07_hand();
void		_int_08_hand();
void		_int_09_hand();
void		_int_0A_hand();
void		_int_0B_hand();
void		_int_0C_hand();
void		_int_0D_hand();
void		_int_0E_hand();
void		_int_0F_hand();
void		_int_10_hand();
void		_int_11_hand();
void		_int_12_hand();
void		_int_13_hand();
void		_int_14_hand();
void		_int_15_hand();
void		_int_16_hand();
void		_int_17_hand();
void		_int_18_hand();
void		_int_19_hand();
void		_int_1A_hand();
void		_int_1B_hand();
void		_int_1C_hand();
void		_int_1D_hand();
void		_int_1E_hand();
void		_int_1F_hand();
void		_int_20_hand();      /* Timer tick */
void		_int_21_hand();	     /* Teclado */
void		_int_80_hand(dword rd_wr, size_t fd, void * buffer, size_t count); /* Maneja el read y el write */


int		_read_scancode();
int		_inport(dword port);
void 		_outport(dword port, dword data);

void 		_turn_cursor_on();
void 		_turn_cursor_off();
void		_move_cursor(unsigned int pos);
void		_out_vga(int port, int value);

void		_debug (void);
#endif

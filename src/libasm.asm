GLOBAL  _read_msw,_lidt
GLOBAL	_read_scancode
GLOBAL  _turn_cursor_on
GLOBAL  _turn_cursor_off
GLOBAL 	_move_cursor
GLOBAL	_int_00_hand
GLOBAL	_int_01_hand
GLOBAL	_int_02_hand
GLOBAL	_int_03_hand
GLOBAL	_int_04_hand
GLOBAL	_int_05_hand
GLOBAL	_int_06_hand
GLOBAL	_int_07_hand
GLOBAL	_int_08_hand
GLOBAL	_int_09_hand
GLOBAL	_int_0A_hand
GLOBAL	_int_0B_hand
GLOBAL	_int_0C_hand
GLOBAL	_int_0D_hand
GLOBAL	_int_0E_hand
GLOBAL	_int_0F_hand
GLOBAL	_int_10_hand
GLOBAL	_int_11_hand
GLOBAL	_int_12_hand
GLOBAL	_int_13_hand
GLOBAL	_int_14_hand
GLOBAL	_int_15_hand
GLOBAL	_int_16_hand
GLOBAL  _int_17_hand
GLOBAL  _int_18_hand
GLOBAL  _int_19_hand
GLOBAL  _int_1A_hand
GLOBAL  _int_1B_hand
GLOBAL  _int_1C_hand
GLOBAL  _int_1D_hand
GLOBAL  _int_1E_hand
GLOBAL  _int_1F_hand
GLOBAL  _int_20_hand
GLOBAL  _int_21_hand
GLOBAL  _int_80_hand
GLOBAL	_inport
GLOBAL  _outport
GLOBAL  _mascaraPIC1,_mascaraPIC2,_Cli,_Sti
GLOBAL  _debug

EXTERN	int_00
EXTERN	int_01
EXTERN	int_02
EXTERN	int_03
EXTERN	int_04
EXTERN	int_05
EXTERN	int_06
EXTERN	int_07
EXTERN	int_08
EXTERN	int_09
EXTERN	int_0A
EXTERN	int_0B
EXTERN	int_0C
EXTERN	int_0D
EXTERN	int_0E
EXTERN	int_0F
EXTERN	int_10
EXTERN	int_11
EXTERN	int_12
EXTERN	int_13
EXTERN	int_14
EXTERN	int_15
EXTERN	int_16
EXTERN	int_17
EXTERN	int_18
EXTERN	int_19
EXTERN	int_1A
EXTERN	int_1B
EXTERN	int_1C
EXTERN	int_1D
EXTERN	int_1E
EXTERN	int_1F
EXTERN  int_20
EXTERN  int_21
EXTERN __write
EXTERN __read
EXTERN getAscii
EXTERN __keyboard_buffer
EXTERN __KBUFFER_PTR_

SECTION .text


_Cli:
	cli			; limpia flag de interrupciones
	ret

_Sti:

	sti			; habilita interrupciones por flag
	ret

_mascaraPIC1:			; Escribe mascara del PIC 1
	push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	21h,al
        pop     ebp
        retn

_mascaraPIC2:			; Escribe mascara del PIC 2
	push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	0A1h,al
        pop     ebp
        retn

_read_msw:
        smsw    ax		; Obtiene la Machine Status Word
        retn


_lidt:				; Carga el IDTR
        push    ebp
        mov     ebp, esp
        push    ebx
        mov     ebx, [ss: ebp + 6] ; ds:bx = puntero a IDTR 
	rol	ebx,16		    	
	lidt    [ds: ebx]          ; carga IDTR
        pop     ebx
        pop     ebp
        retn


_int_00_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax
	
	call	int_00
       	
        iret


_int_01_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_01

        iret


_int_02_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_02
       	
        iret


_int_03_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_03
       	
        iret


_int_04_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_04
       	
        iret


_int_05_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_05
       	mov	eax, 1

        iret


_int_06_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_06
       	
        iret


_int_07_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_07
       	
        iret


_int_08_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_08
       	
        iret


_int_09_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_09
       	
        iret


_int_0A_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_0A
       	
        iret


_int_0B_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_0B
       	
        iret


_int_0C_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_0C
       	
        iret


_int_0D_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_0D
       	
        iret


_int_0E_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_0E
       	
        iret


_int_0F_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_0F
       	
        iret


_int_10_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_10
       	
        iret


_int_11_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_11
       	
        iret


_int_12_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_12
       	
        iret


_int_13_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_13
       	
        iret


_int_14_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_14
       	
        iret


_int_15_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_15
       	
        iret


_int_16_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_16
       	
        iret


_int_17_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_17
       	
        iret


_int_18_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_18
       	
        iret


_int_19_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_19
       	
        iret


_int_1A_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_1A
       	
        iret


_int_1B_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_1B
       	
        iret


_int_1C_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_1C
       	
        iret


_int_1D_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_1D
       	
        iret


_int_1E_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_1E
       	
        iret


_int_1F_hand:				
        mov     ax, 10h			
        mov     ds, ax	
        mov     es, ax	

	call	int_1F
       	
        iret


_int_20_hand:				; Handler de INT 20h (Timer tick)
        push    ds
        push    es                      ; Se salvan los registros
        pusha                           
        mov     ax, 10h			
        mov     ds, ax			; Carga de DS y ES con el valor del selector a utilizar.
        mov     es, ax                  
        call    int_20                 
        mov	al, 20h			; Envio de EOI generico al PIC
	out	20h, al
	popa                            
        pop     es
        pop     ds
        iret

_int_21_hand:				; Handler de INT 21h (Teclado)
        push    ds
        push    es                      ; Se salvan los registros
        pusha      
                     
        mov     ax, 10h			
        mov     ds, ax			; Carga de DS y ES con el valor del selector a utilizar.
        mov     es, ax	

	call	int_21

        mov	al, 20h			; Envio de EOI generico al PIC
	out	20h, al
	popa                            
        pop     es
        pop     ds
        iret





_int_80_hand:
	push	ebp
	mov	ebp, esp
	push    ds
        push    es                      ; Se salvan los registros

	push dword	[ebp+12]		; Pushea los parámetros de read y write
	push dword	[ebp+16]
	push dword	[ebp+20]
	mov	eax, [ebp+8]		; ebp+8 = 0 --> Read. ebp+20 = 1 --> Write
	cmp	eax, 0
	je	read
	jmp	write

continue:
	add	esp, 12	
	pop     es
        pop     ds
	leave
	ret
read:
	call	__read
	jmp	continue
write:
	call	__write
	jmp	continue



_inport:
	push ebp
	mov ebp, esp
	push dx	

	mov eax, 0
	mov dx, [ebp+8]
	in al, dx
	
	pop dx
	leave
	ret

_outport:
	push ebp
	mov ebp, esp
	pusha

	mov dx, [ebp+8]
	mov ax, [ebp+12]
	out dx, al
	
	popa
	leave
	ret

_read_scancode:
	push	ebp
	mov	ebp, esp
	in	al, 60h
	leave
	ret       

_move_cursor:
	push ebp
	mov ebp, esp

	mov cx, [ebp+8]

	mov dx, 3D4h
	mov ax, 0Fh
	out dx, ax

	mov dx, 3D5h
	mov ax, cx
	out dx, ax

	sar cx, 8

	mov dx, 3D4h
	mov ax, 0Eh
	out dx, ax

	mov dx, 3D5h
	mov ax, cx
	out dx, ax


	leave
	ret

_turn_cursor_on:
	push ebp
	mov ebp, esp
	
	mov eax, 0
	mov edx, 0

	mov dx, 3D4h
	mov ax, 0Ah
	out dx, ax

	mov dx, 3D5h
	mov ax, 0h
	out dx, ax


	leave
	ret


_turn_cursor_off:
	push ebp
	mov ebp, esp
	
	mov eax, 0
	mov edx, 0

	mov dx, 3D4h
	mov ax, 0Ah
	out dx, ax

	mov dx, 3D5h
	mov ax, 020h
	out dx, ax


	leave
	ret



; Debug para el BOCHS, detiene la ejecución; Para continuar colocar en el BOCHSDBG: set $eax=0
;


_debug:
        push    bp
        mov     bp, sp
        push	ax
vuelve:	mov     ax, 1
        cmp	ax, 0
	jne	vuelve
	pop	ax
	pop     bp
        retn



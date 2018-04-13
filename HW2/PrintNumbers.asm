        ; 8080 assembler code
        .hexfile PrintNumbers.hex
        .binfile PrintNumbers.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

	; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6

	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        
	;This program prints even numbers from 0 to 50 to the screen

begin:
	LXI SP,stack 	; always initialize the stack pointer
        mvi c, 50	; init C with 50, final value
	mvi b, 0	; init B with 0, initial value
	mvi d, 1	; init D with 1 to use as AND mask
	mvi a, 0	; A = 0, clear

loop:
	MOV a, b	; A = B (A = current number)
	ANA d		; and current number with the mask 0x1
	JNZ increase	; if result is not 0 then number is odd, skip printing

print:
	MVI A, PRINT_B	; print B (current number) screen
	call GTU_OS	; system call

increase:
	INR b		; B = B + 1
	MOV a, c	; A = C
	SUB b		; A = A - B which is actually C-B
	JZ finalize	; if C-B is 0 then finalize the program
	JMP loop	; else jump back to loop

finalize:
	MVI A, PRINT_B	; print final number to screen
	call GTU_OS	; system call
	hlt		; end program

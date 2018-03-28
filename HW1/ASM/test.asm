        ; 8080 assembler code
        .hexfile test.hex
        .binfile test.com
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
GET_RND		equ 7

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

	;This program tests all system calls by calling them

enterInt: dw 'Enter an integer',00AH,00H ; null terminated string
enterStr: dw 'Enter a string',00AH,00H ; null terminated string
rand: dw 'Random number is: ',00AH,00H ; null terminated string


begin:
	LXI SP,stack 	; always initialize the stack pointer

	; PRINT enterInt
	LXI B, enterInt	; prepare text to print
	MVI A, PRINT_STR; print text to screen
	call GTU_OS	; system call
	
	; READ REG B
	MVI A, READ_B	; read value to register B
	call GTU_OS	; system call
	
	; PRINT REG B
	MVI A, PRINT_B	; print the value read from keyboard
	call GTU_OS	; system call
	
	; PRINT enterStr
	LXI B, enterStr	; prepare text to print
	MVI A, PRINT_STR; print text to screen
	call GTU_OS	; system call

	; PREPARE MEM LOCATION
	MVI B, 10H
	MVI C, 0H

	; READ STRING
	MVI A, READ_STR	; read string from screen to mem[BC]
	call GTU_OS	; system call
	
	; PRINT STRING
	MVI A, PRINT_STR;print text pointed by mem[BC] to screen
	call GTU_OS	;system call
	
	; PRINT enterInt
	LXI B, enterInt	; prepare text to print
	MVI A, PRINT_STR; print text to screen
	call GTU_OS	; system call
	
	; PREPARE MEM LOCATION
	MVI B, 10H
	MVI C, 0H
	
	; READ TO MEMORY
	MVI A, READ_MEM	; read integer to mem[BC]
	call GTU_OS	; system call
	
	; PRINT FROM MEMORY
	MVI A, PRINT_MEM; print integer from mem[BC]
	call GTU_OS	; system call
	
	; PRINT rand
	LXI B, rand	; prepare text to print
	MVI A, PRINT_STR; print text to screen
	call GTU_OS	; system call
	
	; PRODUCE RANDOM NUMBER
	MVI A, GET_RND	; generate a random number to register B
	call GTU_OS	; system call

	; PRINT REG B
	MVI A, PRINT_B	; print the value read from keyboard
	call GTU_OS	; system call	

	hlt		; end program


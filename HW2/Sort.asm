        ; 8080 assembler code
        .hexfile Sort.hex
        .binfile Sort.com
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

	;This program produces 50 random integers and sorts them with bubble sort

list: dw  01H, 02H, 03H, 04H, 05H, 06H, 07H, 08H, 09H, 0AH, 0BH, 0CH, 0DH, 0EH, 0FH, 10H, 11H, 12H, 13H, 14H, 15H, 16H, 17H, 18H, 19H, 1AH, 1BH, 1CH, 1DH, 1EH, 1FH, 20H, 21H, 22H, 23H, 24H, 25H, 26H, 27H, 28H, 29H, 2AH, 2BH, 2CH, 2DH, 2EH, 2FH, 30H, 31H, 32H

size: equ 50



begin:
	LXI SP,stack 	; always initialize the stack pointer
	MVI d, size	; get size of array
	LXI h, list	; get array


random:
	MVI a, GET_RND	; get a random integer to register B
	call GTU_OS	; system call
	MOV m, b	; store random number on array
	INX h		; increase address for next element
	INX h
	DCR d		; decrease size to count all elements
	JNZ random	; continue until enough number of elements created
	MVI d, size	; get size of array for outer loop

reload:	
	MVI c, size	; get size of array for inner loop
	LXI h, list	; get array


sortBegin:
	MOV b, m	; get first item B
	INX h		; increase address for next element
	INX h
	MOV a, m	; get second item A
	CMP b		; compare register A and register B
	JC swap		; jump to swap if A<B

sortCont:
	DCR c		; decrease inner class counter
	JNZ sortBegin	; sort again if inner counter is not over
	DCR d		; decrease outer class counter
	JNZ reload	; reload array address and size again


	MVI d, size	; get size to print numbers
	LXI h, list	; get list to print numbers
	
printLoop:
	MOV b, m	; get number to print
	MVI a, PRINT_B	; print number to screen
	call GTU_OS	; system call
	INX h		; increase memory address
	INX h
	DCR d		; decrease loop index
	JNZ printLoop	; loop again if index is not zero
	HLT		; end the program if all counters are done



swap:
	MOV m, b
	DCX h
	DCX h
	MOV m, a
	INX h
	INX h
	JMP sortCont



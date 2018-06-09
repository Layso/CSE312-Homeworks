; 8080 assembler code
.hexfile prog.hex
.binfile prog.com

; try "hex" for downloading in hex format
.download bin  
.objcopy gobjcopy
.postbuild echo "OK!"



; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6
GET_RND		equ 7
TExit		equ 8
TJoin		equ 9
TYield		equ 10
TCreate		equ 11
FileCreate  equ 12
FileClose   equ 13
FileOpen    equ 14
FileRead    equ 15
FileWrite   equ 16
FileSeek    equ 17
DirRead     equ 18



; Position for stack pointer
stack   equ 0F000h

	;Beginning
	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
	pop psw
	pop h
	pop d
	pop D
	ret
;---------------------------------------------------------------------------------------
; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE


ERROR_MESSAGE: dw 'An error occured, closing program', 00AH, 00AH
ASK_FILE_NAME: dw 'Enter a file name',00AH,00H
NEW_LINE: dw '',00AH,00H ; null terminated string
LIST: dw 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50
SIZE equ 50
ZERO equ 0


;Start of the program flow
begin:
	LXI SP, stack		; Initializing stack

	; Asking a file name from user
	LXI B, ASK_FILE_NAME	;
	MVI A, PRINT_STR	;
	call GTU_OS		;
	LXI B, NEW_LINE		; 
	MVI A, PRINT_STR	; 
	call GTU_OS		;
	
	; Getting file name from user
	MVI A, READ_STR		;
	call GTU_OS		;
	PUSH B			;
	
	; Creating file with given name
	MVI A, FileCreate	;
	call GTU_OS		;
	MVI A, ZERO		;
	CMP B			;
	JZ errorEnd		;
	
	; Opening file
	MVI A, FileOpen		;
	POP B			;
	call GTU_OS		;
	MVI A, ZERO		;
	CMP B			;
	JZ errorEnd		;
	
	; Write numbers from 0 to 50 to the file
	MOV E, B		;
	LXI B, LIST		;
	MVI D, 2		;
	MVI H, SIZE		;
	
	; Continue until 50
loop:
	PUSH B			;
	MVI A, FileWrite	;
	call GTU_OS		;
	MVI A, ZERO		;
	CMP B			;
	JZ errorEnd		;
	DCR H			;
	POP B			;
	INX B			;
	INX B			;
	JNZ loop		;

end:
	HLT


errorEnd:
	LXI B, ERROR_MESSAGE
	MVI A, PRINT_STR
	call GTU_OS
	HLT
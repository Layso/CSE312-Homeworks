; 8080 assembler code
.hexfile hw3prog4.hex
.binfile hw3prog4.com

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


WRITE_MSG: dw 'will be written to file', 00AH, 00AH
ERROR_MESSAGE: dw 'An error occured, closing program', 00AH, 00AH
ASK_FILE_NAME: dw 'Enter a file name',00AH,00H
NEW_LINE: dw ' ',00AH,00H ; null terminated string
LIST: dw 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50
APPEND_SIZE equ 10
SIZE equ 50
ZERO equ 0
STRING: dw 'long long string to use as buffer for the get directory information', 00AH, 00AH

;Start of the program flow
begin:
	LXI SP, stack		; Initializing stack

	; Printing directory information
	LXI B, STRING		;
	MVI A, DirRead		;
	call GTU_OS		;
	MVI A, PRINT_STR	;
	call GTU_OS		;

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

	; Opening file
	MVI A, FileOpen		;
	call GTU_OS		;
	MVI A, ZERO		;
	CMP B			;
	JZ errorEnd		;
	
	; Set offset to read last byte
	MOV D, B		;
	LXI B, SIZE		;
	DCX B			;
	MVI A, FileSeek		;
	call GTU_OS		;
	
	; Read last number from the file
	MVI A, FileRead		;
	MOV E, D		;
	MVI D, 1		;
	LXI B, LIST		;
	call GTU_OS		;
	MVI A, ZERO		;
	CMP B			;
	JZ errorEnd		;
	
	; Get number to register B
	LXI B, LIST		;
	MOV H, B		;
	MOV L, C		;
	MOV B, M		;
	MVI C, APPEND_SIZE	;

	; Add 10 more numbers
loop:
	; Prepare next number
	INR B			;
	PUSH B			;
	MOV M, B		;

	; Print the next number to write
	MVI A, PRINT_B		;
	call GTU_OS		;
	MVI A, PRINT_STR	;
	LXI B, WRITE_MSG	;
	call GTU_OS		;

	; Write number to file	
	MVI A, FileWrite	;
	MOV B, H		;
	MOV C, L		;
	MVI D, 1		;
	call GTU_OS		;
	MVI A, ZERO		;
	CMP B			;
	JZ errorEnd		;
		
	; Check to see if 10 numbers are finished
	POP B			;	
	DCR C			;
	MVI A, ZERO		;
	CMP C			;
	JNZ loop		;


	; Printing directory information again to see changes
	LXI B, STRING		;
	MVI A, DirRead		;
	call GTU_OS		;
	MVI A, PRINT_STR	;
	call GTU_OS		;

end:
	HLT


errorEnd:
	LXI B, ERROR_MESSAGE
	MVI A, PRINT_STR
	call GTU_OS
	HLT

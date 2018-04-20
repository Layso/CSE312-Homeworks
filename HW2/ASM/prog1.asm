; 8080 assembler code
.hexfile prog1.hex
.binfile prog1.com

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



; This program creates a thread with F1, waits until it's done, then creates another thread with F2, waits until it's done, then creates another thread with F3 then ends the program



; Start of the program flow
; No more stack initialization required since OS handles it now
begin:
	LXI b, function1	; Loading address of F1 to BC registers
	MVI a, TCreate		; Requesting thread creation with given function
	CALL GTU_OS		; System call for upper instruction
	MVI a, TJoin		; Requesting a blockage until previous thread is done
	CALL GTU_OS		; System call for upper instruction

	LXI b, function2	; Loading address of F2 to BC registers
	MVI a, TCreate		; Requesting thread creation with given function
	CALL GTU_OS		; System call for upper instruction
	MVI a, TJoin		; Requesting a blockage until previous thread is done
	CALL GTU_OS		; System call for upper instruction

	LXI b, function3	; Loading address of F3 to BC registers
	MVI a, TCreate		; Requesting thread creation with given function
	CALL GTU_OS		; System call for upper instruction

end:
	hlt			; Halting the system without waiting for last 
				; This may cause last thread to be terminated without finishing it's job





; Function that prints numbers [0,50) to the screen
function1:
	MVI b, 0		; Specifying start point
	MVI c, 50		; Specifying end point

fn1_loop:
	MVI a, PRINT_B		; Prepare to print current number	print(b)
	CALL GTU_OS		; System call for upper instruction
	INR b			; Increase current number		++b
	MOV a, c		; Get end point to accumulator		a = c
	SUB b			; Subtract b from accumulator		a = a-b
	JNZ fn1_loop		; if (Z != 0) jump fn1_loop else continue
	MVI a, TExit		; Exit the thread
	CALL GTU_OS		; System call for upper instruction





; Function that adds numbers [0, 22] and prints the result
; This actually should add numbers up to 1000 but I couldn't complete it
function2:
	MVI c, 22		; Initialize the maximum number
	MVI a, 0		; Clear the accumulator

fn2_loop:
	ADD c			; Add accumulator with c		a = a+c
	DCR c			; Decrease the maximum number		--c
	JNZ fn2_loop		; if (Z != 0) jump fn2_loop else continue
	MOV b, a		; Moving the result to b register	b = a
	MVI a, PRINT_B		; Printing the result
	CALL GTU_OS		; System call for upper instruction
	MVI a, TExit		; Exit the thread
	CALL GTU_OS		; System call for upper instruction





; Function that prints numbers [50,100) to the screen
function3:
	MVI b, 50		; Specifying start point
	MVI c, 100		; Specifying end point

fn3_loop:
	MVI a, PRINT_B		; Prepare to print current number 	print(b)
	CALL GTU_OS		; System call for upper instruction
	INR b			; Increase current number 		++b
	MOV a, c		; Get end point to accumulator		a = c
	SUB b			; Subtract b from accumulator 		a = a-b
	JNZ fn3_loop		; if (Z != 0) jump fn3_loop else continue
	MVI a, TExit		; Exit the thread
	CALL GTU_OS		; System call for upper instruction





; 8080 assembler code
.hexfile prog3.hex
.binfile prog3.com

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



; This program creates 10 threads with F3, waits until they are all done to let them run in parallel then ends the program



; Start of the program flow
; No more stack initialization required since OS handles it now
begin:
	MVI d, 10		; Load thread count to c register
	
loop_1:
	LXI b, function3	; Loading address of F3 to BC registers
	MVI a, TCreate		; Requesting thread creation with F1
	CALL GTU_OS		; System call for upper instruction
	push b			; Push thread ID to stack
	DCR d			; Decrease thread counter
	JNZ loop_1		; Repeat until all threads are created
	
	MVI d, 10		; Reload thread count to c register
	MVI a, TJoin		; Prepare system call for thread join
loop_2:	
	pop b			; Get thread ID from stack	
	CALL GTU_OS		; System call for thread join
	DCR d			; Decrease thread counter
	JNZ loop_2		; Repeat until all threads are joined

end:
	hlt			; Halting the system after all done





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





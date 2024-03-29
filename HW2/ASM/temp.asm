; 8080 assembler code
.hexfile temp.hex
.binfile temp.com

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



; Defining array and array size
size equ 50
list: dw 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50 





;Start of the program flow
begin:
	LXI SP, stack		; Initializing stack

	LXI b, function1	; Loading function pointer to bc
	MVI a, TCreate		; Loading thread create system code to a
	CALL GTU_OS		; System call for upper instruction
	MOV d, b		; Save thread id on d
	
	LXI b, function3	; Loading function pointer to bc
	MVI a, TCreate		; Loading thread create system code to a
	CALL GTU_OS		; System call for upper instruction
	
	MVI a, TJoin		; Loading thread wait system code to a for second thread
	CALL GTU_OS		; System call for upper instruction
	
	MOV b, d		; Load second thread id to b
	MVI a, TJoin		; Loading thread wait system code to a for first thread
	CALL GTU_OS		; System call for upper instruction

end:
	hlt










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




function4:
	MVI d, size
	LXI h, list

fn4_random:
	MVI a, GET_RND		; Get a random integer			b = rand()
	CALL GTU_OS		; System call for upper instruction	
	MOV m, b		; Store random integer on array		arr[i]=b
	INX h			; Increase address by 2 bytes		++i
	INX h			;					++i
	DCR d			; Decrease remaining item count		--sizeTemp
	JNZ fn4_random		; if (Z != 0) jump fn4_random else continue
	MVI d, size		; Prepare size for outer loop		sizeOut = 50

fn4_reload:
	MVI c, size		; Prepare size for inner loop		sizeInn = 50
	LXI h, list		; Get the beginning of list to HL	i = 0

fn4_sort:
	MOV b, m		; Get current number to b		b = arr[i]
	INX h			; Increase index by 2 bytes		++i
	INX h			;					++i
	MOV a, m		; Get next current number to a		a = arr[i]
	CMP b			; Compare a and b
	JC fn4_swap		; if (C == 1) jump swap else continue	if(a<b) jmp
	
fn4_sort_cont:
	DCR c			; Decrease inner counter		--sizeInn
	JNZ fn4_sort		; Iterate inner loop if inner counter is not 0
	DCR d			; Else decrease outer counter		--sizeOut
	JNZ fn4_reload		; Iterate outer loop if outer counter is not 0
	

	MVI d, size		; Get size of list to d to print array	d = size
	LXI h, list		; Get the beginning of list to HL	i = 0

fn4_print:
	MOV b, m		; Get number to print			b = arr[i]
	MVI a, PRINT_B		; Prepare to print current number	print(arr[i])
	CALL GTU_OS		; System call for upper instruction
	INX h			; Increase index by 2 bytes		++i
	INX h			;					++i
	DCR d			; Decrease remaining item count		--sizeTemp
	JNZ fn4_print		; if (Z != 0) jump fn4_print else continue
	RET			; return	

fn4_swap:
	MOV m, b		; Put bigger number on current index	arr[i] = b
	DCX h			; Decrease index by 2 bytes		--i
	DCX h			;					--i
	MOV m, a		; Put lesser number on previous index	arr[i] = a
	INX h			; Increase index to old position	++i
	INX h			;					++i
	JMP fn4_sort_cont	; Swap completed, continue to sort
	




function5:
	ret


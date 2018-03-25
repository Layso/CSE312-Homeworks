#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"



/* Constant value(s) */
const int BYTE_BIT_COUNT = 8;

uint64_t GTUOS::handleCall(const CPU8080 & cpu){
	int op = 7;
	std::cout << "SysCall recieved: " << (int)cpu.state->a << std::endl;
	
	
	
	switch(op) {
		case GTUOS::PRINT_B :	OperationPrintB(cpu); break;
		case GTUOS::PRINT_MEM :	OperationPrintMem(cpu); break;
		case GTUOS::READ_B :	OperationReadB(cpu); break;
		case GTUOS::READ_MEM :	OperationReadMem(cpu); break;
		case GTUOS::PRINT_STR :	OperationPrintStr(cpu); break;
		case GTUOS::READ_STR :	OperationReadStr(cpu); break;
		case GTUOS::GET_RND :	OperationGetRnd(cpu); break;
		default: std::cout << "Unimplemented systam call\n"; exit(EXIT_FAILURE);
	}
}



void GTUOS::OperationPrintB(const CPU8080 &cpu) {
	int value;
	
	
	/* Reading value from register B */
	value = (int)cpu.state->b;
	
	/* Printing value to standart output */
	std::cout << value << std::endl;
}



void GTUOS::OperationPrintMem(const CPU8080 &cpu) {
	uint16_t address;
	int value;
	
	
	/* Reading value from memory */
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	value = (int)cpu.memory->at(address);
	
	/* Printing value to standart output */
	std::cout << value << std::endl;
}



void GTUOS::OperationReadB(const CPU8080 &cpu) {
	/* 
	 * TODO:
	 * Read an integer from stdin
	 * Write integer to register B
	 */
}



void GTUOS::OperationReadMem(const CPU8080 &cpu) {
	/*
	 * TODO:
	 * Read an integer from stdin
	 * Write integer to mem[CB]
	 */
}



void GTUOS::OperationPrintStr(const CPU8080 &cpu) {
	/*
	 * TODO:
	 * Get string from mem[BC]
	 * Print string to the stdout
	 */
}



void GTUOS::OperationReadStr(const CPU8080 &cpu) {
	/*
	 * TODO:
	 * Get string from stdin
	 * Write string to the mem[BC]
	 */
}



void GTUOS::OperationGetRnd(const CPU8080 &cpu) {
	/*
	 * TODO:
	 * Produce a random integer
	 * Place random integer to register B
	 */	
}



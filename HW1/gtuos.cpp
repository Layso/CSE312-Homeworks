#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"





uint64_t GTUOS::handleCall(const CPU8080 & cpu){
	std::cout << "SysCall recieved\n";
	
	
	switch(cpu.state->a) {
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
	
}



void GTUOS::OperationPrintMem(const CPU8080 &cpu) {
	
}



void GTUOS::OperationReadB(const CPU8080 &cpu) {
	
}



void GTUOS::OperationReadMem(const CPU8080 &cpu) {
	
}



void GTUOS::OperationPrintStr(const CPU8080 &cpu) {
	
}



void GTUOS::OperationReadStr(const CPU8080 &cpu) {
	
}



void GTUOS::OperationGetRnd(const CPU8080 &cpu) {
	
}



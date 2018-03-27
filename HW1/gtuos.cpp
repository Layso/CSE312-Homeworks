#include <iostream>
#include <ctime>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
#include <string>


/* Constant value(s) */
const int BYTE_BIT_COUNT = 8;

uint64_t GTUOS::handleCall(const CPU8080 & cpu){
	int cycleCount;
	std::cout << "SysCall recieved: " << (int)cpu.state->a << std::endl;
	

	switch(cpu.state->a) {
		case GTUOS::PRINT_B :	cycleCount = OperationPrintB(cpu); break;
		case GTUOS::PRINT_MEM :	cycleCount = OperationPrintMem(cpu); break;
		case GTUOS::READ_B :	cycleCount = OperationReadB(cpu); break;
		case GTUOS::READ_MEM :	cycleCount = OperationReadMem(cpu); break;
		case GTUOS::PRINT_STR :	cycleCount = OperationPrintStr(cpu); break;
		case GTUOS::READ_STR :	cycleCount = OperationReadStr(cpu); break;
		case GTUOS::GET_RND :	cycleCount = OperationGetRnd(cpu); break;
		default: std::cout << "Unimplemented systam call\n"; exit(EXIT_FAILURE);
	}
	
	
	return cycleCount;
}



int GTUOS::OperationPrintB(const CPU8080 &cpu) {
	const int cycle = 10;
	int value;
	
	
	/* Reading value from register B */
	value = (int)cpu.state->b;
	
	/* Printing value to standart output */
	std::cout << value << std::endl;
	
	
	return cycle;
}



int GTUOS::OperationPrintMem(const CPU8080 &cpu) {
	const int cycle = 10;
	uint16_t address;
	int value;
	
	
	/* Reading value from memory */
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	value = (int)cpu.memory->at(address);
	
	/* Printing value to standart output */
	std::cout << value << std::endl;
	
	
	return cycle;
}



int GTUOS::OperationReadB(const CPU8080 &cpu) {
	const int cycle = 10;
	int value;
	
	
	/* Reading value from standart input */
	std::cin >> value;
	
	/* Writing value to register B */
	cpu.state->b = (uint8_t)value;
	
	
	return cycle;
}



int GTUOS::OperationReadMem(const CPU8080 &cpu) {
	const int cycle = 10;
	uint16_t address;
	int value;
	
	
	/* Reading value from standart input */
	std::cin >> value;
	
	/* Writing value to memory */
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	cpu.memory->at(address) = (uint8_t)value;
	
	
	return cycle;
}



int GTUOS::OperationPrintStr(const CPU8080 &cpu) {
	const int cycle = 100;
	uint16_t address;
	
	
	/* Calculating memory address */
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	
	/* Printing memory content until a null character found */
	while(cpu.memory->at(address) != '\0') {
		std::cout << cpu.memory->at(address++);
	}
	
	/* Ending line */
	std::cout << std::endl;
	
	
	return cycle;
}



int GTUOS::OperationReadStr(const CPU8080 &cpu) {
	const int cycle = 100;
	std::string input;
	uint16_t address;
	
	
	/* Calculating memory address */
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	
	/* Reading value from standart input */
	std::getline(std::cin, input);
	
	/* Writing input byte by byte to memory */
	for (int i=0; i<input.length(); ++i) {
		cpu.memory->at(address+i) = input[i];
	}
	
	
	return cycle;
}



int GTUOS::OperationGetRnd(const CPU8080 &cpu) {
	const int cycle = 5;
	
	
	/* Seeding random with time */
	srand(time(nullptr));
	
	/* Writing random number to register B */
	cpu.state->b = rand();
	
	
	return cycle;
}


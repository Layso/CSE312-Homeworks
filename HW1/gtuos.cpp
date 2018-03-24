#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

uint64_t GTUOS::handleCall(const CPU8080 & cpu){
	std::cout << "Derdin ne evladım senin?\n";
	std::cout << cpu.lastOpcode << std::endl;
	return 0;
}

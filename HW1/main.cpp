#include <iostream>
#include <fstream>
#include <string>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"

	// This is just a sample main function, you should rewrite this file to handle problems 
	// with new multitasking and virtual memory additions.
	
void SaveMemory(const MemoryBase &mem);


int main (int argc, char**argv)
{
	if (argc != 3){
		std::cerr << "Usage: prog exeFile debugOption\n";
		exit(1); 
	}
	
	
	int totalCycle = 0;
	int DEBUG = atoi(argv[2]);
	char line;
	
	memory 	mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS;

	theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);	
 
	do	
	{
		totalCycle += theCPU.Emulate8080p(DEBUG);
		if(theCPU.isSystemCall())
			totalCycle += theOS.handleCall(theCPU);
			
		if (DEBUG == 2)
			std::cin >> line;
			
	}	while (!theCPU.isHalted());
	
	theOS.Hexdump(theCPU);
	std::cout << "Total number of cycles: " << totalCycle << std::endl;
	return 0;
}




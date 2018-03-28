#include <iostream>
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
	
	memory 	mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS;

	theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);	
 	/* Seeding random with time */
	srand(time(nullptr));
	do	
	{
		totalCycle += theCPU.Emulate8080p(DEBUG);
		if(theCPU.isSystemCall())
			totalCycle += theOS.handleCall(theCPU);
			
		if (DEBUG == 2)
			std::cin.get();
			
	}	while (!theCPU.isHalted());
	
	std::cout << "Total number of cycles: " << totalCycle << std::endl;
	theOS.Hexdump(theCPU);
	return 0;
}




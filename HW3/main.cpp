#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
#include "FileSystem.h"


	// This is just a sample main function, you should rewrite this file to handle problems 
	// with new multitasking and virtual memory additions.
	
void SaveMemory(const MemoryBase &mem);


int main (int argc, char**argv)
{
	if (argc != 4){
		std::cerr << "Usage: " << argv[0] << " exeFile diskFile debugOption\n";
		exit(1); 
	}
	
	
	int totalCycle = 0;
	int DEBUG = atoi(argv[3]);
	FileSystem disk;
	
	memory 	mem;
	CPU8080 theCPU(&mem);
	disk.ReadSystemFile(argv[2]);
	theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);	
	GTUOS	theOS(theCPU, DEBUG, disk);
	
 	/* Seeding random with time */
	srand(time(NULL));
	
	
	do	
	{
		theOS.CheckStackInstruction(theCPU);
		
		if (!theOS.StackOverFlow(theCPU)) {
			totalCycle += theCPU.Emulate8080p(DEBUG);
			if(theCPU.isSystemCall())
				totalCycle += theOS.handleCall(theCPU);
		}
		
		
		theOS.EndOfCycleCheck(theCPU, totalCycle);
	}	while (!theCPU.isHalted());
	
	theOS.Hexdump(theCPU);
	
	std::cout << "Total number of cycles: " << totalCycle << std::endl;
	return 0;
}




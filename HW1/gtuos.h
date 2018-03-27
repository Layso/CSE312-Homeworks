#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"



class GTUOS{
	public:
		uint64_t handleCall(const CPU8080 & cpu);
		void Hexdump(const CPU8080 &cpu);
		
		/* Const definitions for opcodes to compare */
		enum Constants {
            PRINT_B = 1,
            PRINT_MEM = 2,
			READ_B = 3,
			READ_MEM = 4,
			PRINT_STR = 5,
			READ_STR = 6,
			GET_RND = 7
        };
				
	private:
		/* Functions to operate according to system call */
		int OperationPrintB(const CPU8080 &cpu);
		int OperationPrintMem(const CPU8080 &cpu);
		int OperationReadB(const CPU8080 &cpu);
		int OperationReadMem(const CPU8080 &cpu);
		int OperationPrintStr(const CPU8080 &cpu);
		int OperationReadStr(const CPU8080 &cpu);
		int OperationGetRnd(const CPU8080 &cpu);

};

#endif

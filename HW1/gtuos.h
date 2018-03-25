#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"



class GTUOS{
	public:
		uint64_t handleCall(const CPU8080 & cpu);
		
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
		void OperationPrintB(const CPU8080 &cpu);
		void OperationPrintMem(const CPU8080 &cpu);
		void OperationReadB(const CPU8080 &cpu);
		void OperationReadMem(const CPU8080 &cpu);
		void OperationPrintStr(const CPU8080 &cpu);
		void OperationReadStr(const CPU8080 &cpu);
		void OperationGetRnd(const CPU8080 &cpu);
};

#endif

#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"
#include <vector>



/* Constant values */
const int ZERO = 0;
const int NONE = -1;
const int QUANTUM = 100;
const int BYTE_BIT_COUNT = 8;
const int FIRST_THREAD_ID = 2;
const int MEM_START = 0x0f000;
const int MEM_INTERVAL = 0x0800;



class GTUOS {
	public:
		/* Const definitions for opcodes to compare */
		enum Constants {
            PRINT_B = 1,
            PRINT_MEM = 2,
			READ_B = 3,
			READ_MEM = 4,
			PRINT_STR = 5,
			READ_STR = 6,
			GET_RND = 7,
			THREAD_EXIT = 8,
			THREAD_JOIN = 9,
			THREAD_YIELD = 10,
			THREAD_CREATE = 11
        };
		
		
		/* Constructor(s) */
		GTUOS(const CPU8080 &cpu, int DEBUG);
		
		/* Public functions */
		void EndOfCycleCheck(CPU8080 &cpu, int currentCycle);
		uint64_t handleCall(const CPU8080 &cpu);
		void Hexdump(const CPU8080 &cpu);
		
		
		
		/* Inner class for thread accountings */
		class Thread {
			public:
				/* Const definitions for states to compare */
				enum ThreadState {
					RUNNING = 1,
					READY = 2,
					BLOCKED = 3,
					TERMINATED = 4
				};
				
				
				Thread(int _id, int _cycleStart, int _stackStart, const State8080 &_cpuState);
				
				int GetID();
				void GiveTurn(int cycle);
				bool TurnEnded(int cycle);
				bool Terminated();
				bool Blocked();
				void WaitFor(int id, int currentCycle);
				void Notify(int currentCycle);
				int GetWaitedID();
				void SetState(ThreadState newState);
				State8080 GetRegisters();
				void SetRegisters(State8080 _cpuState);
				void AddCycle(int cycles);
				int GetStackPosition();
				ThreadState GetState();
			private:
				int id;
				int turn;
				int turnStart;
				int cycleStart;
				int cycleTotal;
				int blockStart;
				int blockTotal;
				int waitingFor;
				int stackStart;
				ThreadState state;
				State8080 cpuState;
		};
	private:
		/* Private member variables */
		int debugMode;
		int totalTurn;
		int previousCycle;
		Thread runningThread;
		std::vector<Thread> runQueue;
		std::vector<Thread> allThreads;
		std::vector<Thread> terminatedThreads;
		
		
		
		/* Helper functions */
		void SwitchThread();
		bool SearchThread(std::vector<Thread> list, int id);
		int GetNewStackPosition();
		
		/* Functions to operate according to system call */
		int OperationPrintB(const CPU8080 &cpu);
		int OperationPrintMem(const CPU8080 &cpu);
		int OperationReadB(const CPU8080 &cpu);
		int OperationReadMem(const CPU8080 &cpu);
		int OperationPrintStr(const CPU8080 &cpu);
		int OperationReadStr(const CPU8080 &cpu);
		int OperationGetRnd(const CPU8080 &cpu);
		int OperationThreadCreate(const CPU8080 &cpu);
		int OperationThreadJoin(const CPU8080 &cpu);
		int OperationThreadExit(const CPU8080 &cpu);
		int OperationThreadYield(const CPU8080 &cpu);
};

#endif

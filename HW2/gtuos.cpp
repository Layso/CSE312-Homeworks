#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <ctime>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"



using std::cout;
using std::endl;


/* Constructor for GTU OS */
GTUOS::GTUOS(const CPU8080 &cpu, int DEBUG) : runningThread(-1, -1, *cpu.state) {
	previousCycle = ZERO;
	totalTurn = ZERO;
	debugMode = DEBUG;
	cpu.state->sp = STACK_START;
	runningThread = Thread(threadTable.size()+1, previousCycle, *cpu.state);
	threadTable.push_back(runningThread);
	runningThread.SetState(RUNNING);
}



void GTUOS::EndOfCycleCheck(CPU8080 &cpu, int currentCycle) {
	int deadlockCounter = ZERO;
	bool contextSwitch = true;
	int pastCycles = currentCycle - previousCycle;


	runningThread.AddCycle(pastCycles);
	runningThread.SetRegisters(*cpu.state);	
	
	/* If thread's time is up or terminated, get a new thread to use CPU */
	if (runQueue.size() > 0 && (runningThread.Blocked() || runningThread.Terminated() || runningThread.TurnEnded(previousCycle))) {
		SwitchThread(cpu);
		free(cpu.state);
		cpu.state = new State8080(runningThread.GetRegisters());
	}
	
	/* If runQueue is empty terminate the system */
	else if (runQueue.size() == 0 && runningThread.Terminated()) {
		std::cout << "No runnable threads available. Shutting down the system\n";
		*cpu.lastOpcode = 0x76;
	}
	
	previousCycle = currentCycle;
}



/* Helper function to switch the thread on CPU */
void GTUOS::SwitchThread(const CPU8080 &cpu) {
	/* Printing switch information according to debug mode */
	if (debugMode == 2) {
		std::cout << "\nControlling thread switch due to";
	}
	
	
	/* If current thread is terminated insert to terminated list */
	if (runningThread.Terminated()) {
		terminatedThreads.push_back(runningThread);
		
		if (debugMode == 2)
			std::cout << " termination\n";
	}
	
	/* If blocked simply push back to the queue */ 
	else if (runningThread.Blocked()) {
		runQueue.push_back(runningThread);
		
		if (debugMode == 2)
			std::cout << " blocking call\n";
	}
	
	/* If yielded, change state to ready and push back to queue */
	else if (runningThread.Yielded()) {
		runningThread.SetState(READY);
		runQueue.push_back(runningThread);
		
		if (debugMode == 2)
			std::cout << " CPU yielding\n";
	}
	
	else {
		runningThread.SetState(READY);
		runQueue.push_back(runningThread);
		
		if (debugMode == 2)
			std::cout << " end of turn\n";
	}
	
	if (debugMode == 2) {
		for (int i=0; i<runQueue.size(); ++i) {
			if (runQueue[i].Blocked())
				std::cout << "Thread " << runQueue[i].GetID() << " is waiting thread " << runQueue[i].GetWaitedID() << " for " << runQueue[i].HowMuchWaited(previousCycle) << " cycles\n";
		}
		
		std::cout << "Previous thread ID: " << runningThread.GetID() << std::endl;
	}
	
	
	/* Updating thread table for previous thread */
	for (int i=0; i<threadTable.size(); ++i) {
		if (threadTable[i].GetID() == runningThread.GetID()) {
			threadTable.erase(threadTable.begin() + i);
			threadTable.push_back(runningThread);
			break;
		}
	}
	
	
	/* Find the next thread available */
	do {
		runningThread = runQueue.front();			
		runQueue.erase(runQueue.begin());
		if (runningThread.Blocked()) {
			if (SearchThread(terminatedThreads, runningThread.GetWaitedID()))
				runningThread.Notify(previousCycle);
			else
				runQueue.push_back(runningThread);
		}
	} while (runningThread.GetState() == BLOCKED);
	
	
	/* If no available threads found to run terminate the system */
	if (runningThread.GetState() != READY) {
		std::cout << "No runnable threads available. Shutting down the system\n";
		*cpu.lastOpcode = 0x76;
	}
	
	else {
		runningThread.GiveTurn(previousCycle);
		
		/* Updating thread table for next thread */
		for (int i=0; i<threadTable.size(); ++i) {
			if (threadTable[i].GetID() == runningThread.GetID()) {
				threadTable.erase(threadTable.begin() + i);
				threadTable.push_back(runningThread);
				break;
			}
		}
		
		
	}
	
	if (debugMode == 3) {
		std::string state;
		std::cout << "\nThread ID  " << "  Start Cycle  " << "  Cycle Drain  " << "  Thread State  " << "  Memory Start  " << "  Program Counter  " << "  Stack Usage\n";
		for (int i=0; i<threadTable.size(); ++i) {
			switch (threadTable[i].GetState()) {
				case READY: state = "Ready"; break;
				case RUNNING: state = "Running"; break;
				case BLOCKED: state = "Blocked"; break;
				case YIELDED: state = "Yielded"; break;
				case TERMINATED: state = "Terminated"; break;
			}
			
			std::cout << std::setw(9) << threadTable[i].GetID();
			std::cout << std::setw(15) << threadTable[i].GetStartCycle();
			std::cout << std::setw(15) << threadTable[i].GetDrainedCycles();
			std::cout << std::setw(16) << state;
			std::cout << std::setw(16) << threadTable[i].GetMemoryStart();
			std::cout << std::setw(19) << threadTable[i].GetProgramCounter();
			std::cout << std::setw(7) << threadTable[i].GetUsedStack() << " byte(s)"; 
			std::cout << std::endl;
		}
		
		std::cout << std::endl;
	}
	
	if (debugMode == 2)
		std::cout << "Current thread ID:  " << runningThread.GetID() << std::endl << std::endl;
}



/* Function to prevent user change stack position */
void GTUOS::CheckStackInstruction(const CPU8080 &cpu) {
	/* If current instruction changes stack pointer get new instruction by changing pc */
	if (cpu.memory->at(cpu.state->pc) == 0x31)
		cpu.state->pc += 3;
}



/* Contorlling stack overflow to prevent data override */
bool GTUOS::StackOverFlow(const CPU8080 &cpu) {
	if (runningThread.GetUsedStack() > STACK_SIZE) {
		runningThread.SetState(TERMINATED);
		std::cout << "\nStackOverFlow exception. Thread " << runningThread.GetID() << " terminated due to stack overflow\n\n";
		return true;
	}
	
	return false;
}



/* Helper function to search a thread in any thread list */
bool GTUOS::SearchThread(std::vector<Thread> list, int id) {
	for (int i=0; i<list.size(); ++i) {
		if (list[i].GetID() == id)
			return true;
	}
	
	return false;
}



/* Helper function to find an empty stack location for new thread */
int GTUOS::GetNewStackPosition() {
	int newPosition = STACK_START;
	bool flag;
	
	runQueue.push_back(runningThread);
	
	do {
		flag = true;
		for(int i=0; i<runQueue.size(); ++i) {
			if (runQueue[i].GetStackPosition() == newPosition) {
				newPosition -= STACK_SIZE;
				flag = false;
			}
		}
	} while(!flag);
	
	runQueue.pop_back();
	
	
	/* If there is no more memory left return error code */
	if (newPosition < STACK_LIMIT)
		return -1;
	
	return newPosition;
}











/* THREAD IMPLEMENTATIONS */
/* Constructor for thread class */
GTUOS::Thread::Thread(int _id, int _cycleStart, const State8080 &_cpuState) {
	id = _id;
	cpuState = _cpuState;
	cycleStart = _cycleStart;
	stackStart = cpuState.sp;
	memoryStart = _cpuState.pc;
	
	turn = ZERO;
	cycleTotal = ZERO;
	waitingFor = NONE;
	blockStart = NONE;
	blockTotal = ZERO;
	state = READY;
}

bool GTUOS::Thread::TurnEnded(int cycle) {
	return (cycle - turnStart) > QUANTUM;
}

void GTUOS::Thread::GiveTurn(int cycle) {
	turnStart = cycle;
	state = RUNNING;
}

bool GTUOS::Thread::Terminated() {
	return state == TERMINATED;
}

bool GTUOS::Thread::Blocked() {
	return state == BLOCKED;
}

bool GTUOS::Thread::Yielded() {
	return state == YIELDED;
}

int GTUOS::Thread::GetID() {
	return id;
}

void GTUOS::Thread::WaitFor(int id, int currentCycle) {
	waitingFor = id;
	state = BLOCKED;
	blockStart = currentCycle;
}

void GTUOS::Thread::Notify(int currentCycle) {
	blockTotal += currentCycle - blockStart;
	blockStart = NONE;
	waitingFor = NONE;
	state = READY;
}

int GTUOS::Thread::GetWaitedID() {
	return waitingFor;
}

int GTUOS::Thread::GetStartCycle() {
	return cycleStart;
}

int GTUOS::Thread::GetDrainedCycles() {
	return cycleTotal;
}

int GTUOS::Thread::GetMemoryStart() {
	return memoryStart;
}

int GTUOS::Thread::GetProgramCounter() {
	return cpuState.pc;
}

int GTUOS::Thread::GetUsedStack() {
	return stackStart-cpuState.sp;
}

int GTUOS::Thread::HowMuchWaited(int cycle) {
	return cycle - blockStart;
}

void GTUOS::Thread::AddCycle(int cycles) {
	cycleTotal += cycles;
}

State8080 GTUOS::Thread::GetRegisters() {
	return cpuState;
}

void GTUOS::Thread::SetRegisters(State8080 _cpuState) {
	cpuState = _cpuState;
}

ThreadState GTUOS::Thread::GetState() {
	return state;
}

void GTUOS::Thread::SetState(ThreadState newState) {
	state = newState;
}

int GTUOS::Thread::GetStackPosition() {
	return stackStart;
}

int GTUOS::Thread::GetExitStatus() {
	return exitStatus;
}

void GTUOS::Thread::SetExitStatus(int status) {
	exitStatus = status;
}










/* GTUOS IMPLEMENTATIONS */
/* Function to handle system calls */
uint64_t GTUOS::handleCall(const CPU8080 & cpu) {
	int cycleCount;

	switch(cpu.state->a) {
		case GTUOS::PRINT_B :		cycleCount = OperationPrintB(cpu); break;
		case GTUOS::PRINT_MEM :		cycleCount = OperationPrintMem(cpu); break;
		case GTUOS::READ_B :		cycleCount = OperationReadB(cpu); break;
		case GTUOS::READ_MEM :		cycleCount = OperationReadMem(cpu); break;
		case GTUOS::PRINT_STR :		cycleCount = OperationPrintStr(cpu); break;
		case GTUOS::READ_STR :		cycleCount = OperationReadStr(cpu); break;
		case GTUOS::GET_RND :		cycleCount = OperationGetRnd(cpu); break;
		case GTUOS::THREAD_CREATE :	cycleCount = OperationThreadCreate(cpu); break;
		case GTUOS::THREAD_JOIN :	cycleCount = OperationThreadJoin(cpu); break;
		case GTUOS::THREAD_YIELD :	cycleCount = OperationThreadYield(cpu); break;
		case GTUOS::THREAD_EXIT :	cycleCount = OperationThreadExit(cpu); break;
		default: std::cout << "Unimplemented systam call\n"; exit(EXIT_FAILURE);
	}
	
	
	return cycleCount;
}

int GTUOS::OperationThreadCreate(const CPU8080 &cpu) {
	const int cycle = 80;
	State8080 newState;
	uint16_t address;
	int newSP;
	
	/* Calculating the address of the given function */
	memset(&newState, ZERO, sizeof(newState));
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	newSP = GetNewStackPosition();


	if (newSP == -1) {
		std::cout << "\nOutOfMemory exception. Thread creation failed due to low remaining memory\n\n";
		cpu.state->b = 0;
	}
	
	/* Creating new thread object with empty state pointing to desired function as pc, thread id stored to register b */
	else {
		newState.pc = address;
		newState.sp = newSP;
		Thread newThread(threadTable.size()+1, previousCycle+cycle, newState);
		cpu.state->b = newThread.GetID();
		threadTable.push_back(newThread);
		runQueue.push_back(newThread);
	}
	
	
	return cycle;
}



int GTUOS::OperationThreadJoin(const CPU8080 &cpu) {
	const int cycle = 40;
	
	
	/* Controlling if there is a valid thread to wait to prevent deadlock */
	if (SearchThread(runQueue, cpu.state->b) || SearchThread(terminatedThreads, cpu.state->b)) {
		runningThread.WaitFor(cpu.state->b, previousCycle + cycle);
	}
	
	return cycle;
}



int GTUOS::OperationThreadExit(const CPU8080 &cpu) {
	const int cycle = 50;
	
	runningThread.SetExitStatus(cpu.state->b);
	runningThread.SetState(TERMINATED);
	
	return cycle;
}



int GTUOS::OperationThreadYield(const CPU8080 &cpu) {
	const int cycle = 40;
	
	std::cout << "Thread yielding\n";
	runningThread.SetState(YIELDED);
	
	return cycle;
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
	
	return cycle;
}



int GTUOS::OperationReadStr(const CPU8080 &cpu) {
	const int cycle = 100;
	int i;
	std::string input;
	uint16_t address;
	char c;
	
	
	/* Calculating memory address */
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	
	/* Reading value from standart input and appending with both new line and null characters */
	std::cin.ignore();
	std::getline(std::cin, input);
	input.append("\n\0");
	
	/* Writing input byte by byte to memory */
	for (i=0; i<input.length(); ++i) {
		cpu.memory->at(address+i) = input[i];
	}
	
	
	return cycle;
}



int GTUOS::OperationGetRnd(const CPU8080 &cpu) {
	const int cycle = 5;
		
	/* Writing random number to register B */
	cpu.state->b = (uint8_t)rand();
		
	return cycle;
}



void GTUOS::Hexdump(const CPU8080 &cpu) {
	const std::string fileName = "exe.mem";
	const int lineCellCount = 16;
	const int memoryCap = 0x10000;
	uint16_t memoryContent;
	
	
	/* Opening file to write */
	std::ofstream outputFile;
	outputFile.open(fileName);
	
	/* Writing memory content to file with a nice format */
	for (int i=0; i<memoryCap; ++i) {
		memoryContent = cpu.memory->at(i);
		
		if (!(i%lineCellCount)) {
			if (i!=0)
				outputFile << std::endl;
			
			outputFile << "0x" << std::hex << std::setw(5) << std::setfill('0') << i << ":\t";
		}
		
		outputFile << "\t" << memoryContent << " ";
	}
	
	/* Closing file */
	outputFile.close();
}

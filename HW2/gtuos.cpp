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
GTUOS::GTUOS(const CPU8080 &cpu, int DEBUG) : runningThread(FIRST_THREAD_ID, totalTurn, MEM_START, *cpu.state) {
	previousCycle = ZERO;
	totalTurn = ZERO;
	debugMode = DEBUG;
	allThreads.push_back(runningThread);
	runningThread.SetState(Thread::ThreadState::RUNNING);
}



void GTUOS::EndOfCycleCheck(CPU8080 &cpu, int currentCycle) {
	int deadlockCounter = ZERO;
	bool contextSwitch = true;
	int pastCycles = currentCycle - previousCycle;
	runningThread.AddCycle(pastCycles);
	
	/*cout << endl;
	cout << "Blocked:  " << runningThread.Blocked() << endl;
	cout << "Termi:    " << runningThread.Terminated() << endl;
	cout << "Turn end: ";
	runningThread.TurnEnded(previousCycle);
	cout << "Queue:    " << runQueue.size() << endl;*/
	/* If thread's time is up or terminated, get a new thread to use CPU */
	if (runQueue.size() > 0 && (runningThread.Blocked() || runningThread.Terminated() || runningThread.TurnEnded(previousCycle))) {
		runningThread.SetRegisters(*cpu.state);
		SwitchThread();
		free(cpu.state);
		cpu.state = new State8080(runningThread.GetRegisters());
	}
	
	
	previousCycle = currentCycle;
}



/* Helper function to switch the thread on CPU */
void GTUOS::SwitchThread() {
	/* Printing switch information according to debug mode */
	if (debugMode == 2 || debugMode == 0 || debugMode == 1) {
		std::cout << "Swtiching thread  " << runningThread.GetID() << " because";
	}
	
	
	/* If current thread is terminated insert to terminated list */
	if (runningThread.Terminated()) {
		terminatedThreads.push_back(runningThread);
		std::cout << " terminated\n";
	}
	
	else if (runningThread.Blocked()) {
		runQueue.push_back(runningThread);
		std::cout << " blocked\n";
	}
	
	else {
		runningThread.SetState(Thread::READY);
		runQueue.push_back(runningThread);
		std::cout << " turn ended\n";
	}
	
	//cout << "Current thread: " << runningThread.GetID() << "/" << previousCycle << endl;
	do {
		runningThread = runQueue.front();
		runQueue.erase(runQueue.begin());
		if (runningThread.Blocked()) {
			if (SearchThread(terminatedThreads, runningThread.GetWaitedID()))
				runningThread.Notify(previousCycle);
			else
				runQueue.push_back(runningThread);
		}
		
	} while (runningThread.GetState() == Thread::ThreadState::BLOCKED);
	runningThread.GiveTurn(previousCycle);
	
	cout << "Currently running: " << runningThread.GetID() << endl;
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
	int newPosition = MEM_START;
	bool flag;
	
	runQueue.push_back(runningThread);
	
	do {
		flag = true;
		for(int i=0; i<runQueue.size(); ++i) {
			if (runQueue[i].GetStackPosition() == newPosition) {
				newPosition -= MEM_INTERVAL;
				flag = false;
			}
		}
		
		
	} while(!flag);
	
	runQueue.pop_back();
	
	return newPosition;
}











/* THREAD IMPLEMENTATIONS */
/* Constructor for thread class */
GTUOS::Thread::Thread(int _id, int _cycleStart, int _stackStart, const State8080 &_cpuState) {
	id = _id;
	cpuState = _cpuState;
	cycleStart = _cycleStart;
	stackStart = _stackStart;
	cpuState.sp = _stackStart;
	
	turn = ZERO;
	cycleTotal = ZERO;
	waitingFor = NONE;
	blockStart = NONE;
	blockTotal = ZERO;
	state = ThreadState::READY;
}



bool GTUOS::Thread::TurnEnded(int cycle) {
	//cout << cycle << "/" << turnStart << endl;
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

void GTUOS::Thread::AddCycle(int cycles) {
	cycleTotal += cycles;
}

State8080 GTUOS::Thread::GetRegisters() {
	return cpuState;
}

void GTUOS::Thread::SetRegisters(State8080 _cpuState) {
	cpuState = _cpuState;
}

GTUOS::Thread::ThreadState GTUOS::Thread::GetState() {
	return state;
}

void GTUOS::Thread::SetState(ThreadState newState) {
	state = newState;
}

int GTUOS::Thread::GetStackPosition() {
	return stackStart;
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
	
	
	/* Calculating the address of the given function */
	memset(&newState, ZERO, sizeof(newState));
	address = cpu.state->b;
	address = (address << BYTE_BIT_COUNT) | cpu.state->c;
	newState.pc = address;
	
	
	
	/* Creating new thread object with empty state pointing to desired function as pc, thread id stored to register b */
	Thread newThread(allThreads.size()+2, previousCycle+cycle, GetNewStackPosition(), newState);
	cpu.state->b = newThread.GetID();
	allThreads.push_back(newThread);
	runQueue.push_back(newThread);
	
	std::cout << "Thread " << newThread.GetID() << " created\n";
	
	return cycle;
}



int GTUOS::OperationThreadJoin(const CPU8080 &cpu) {
	const int cycle = 40;
	
	
	/* Controlling if there is a valid thread to wait to prevent deadlock */
	if (SearchThread(runQueue, cpu.state->b) || SearchThread(terminatedThreads, cpu.state->b)) {
		int id = cpu.state->b;
		std::cout << "Waiting for: " << id << std::endl;
		runningThread.WaitFor(cpu.state->b, previousCycle + cycle);
	}
	
	
	return cycle;
}



int GTUOS::OperationThreadExit(const CPU8080 &cpu) {
	const int cycle = 50;
	
	
	cout << "Thread " << runningThread.GetID() << " used TExit\n";
	runningThread.SetState(Thread::ThreadState::TERMINATED);
	
	
	return cycle;
}



int GTUOS::OperationThreadYield(const CPU8080 &cpu) {
	const int cycle = 40;
	std::cout << "Thread yielding\n";
	
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
	
	/* Ending line */
	/*std::cout << std::endl;*/
	
	
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

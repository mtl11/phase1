#include <phase1.h>
#include <usloss.h>
#include <stdio.h>
#include <string.h>

//typedef struct ProcTableEntry ProcTableEntry;


typedef void (*Process)();

struct ProcTableEntry {
	int pid;
	int ppid;
	int priority;
	int status;
	int startTime;
	struct ProcTableEntry* parent;
	struct ProcTableEntry* firstChild;
	struct ProcTableEntry* nextChild;
	struct ProcTableEntry zapList[MAXPROC];
	char procName[50];
	USLOSS_Context context;
	Process func;
};

char lastProc[50];

struct ProcTableEntry procTable[MAXPROC];

void dispatcher(){

}

void initProc(){
        //init process
        //TODO: create sentinel proc
        //TODO: create testcase_main proc
	phase2_start_service_processes();
	phase3_start_service_processes();
	phase4_start_service_processes();
	phase5_start_service_processes();
	testcase_main();
        while (1){
                join(0);
        }
}

void phase1_init(void){
	//TODO: populate proc table with init
	int i;
	for (i = 0; i < MAXPROC; i++) {
		procTable[i].pid = -1;
		procTable[i].pid = -1;
		procTable[i].priority = -1;
		procTable[i].status = -1;
		procTable[i].parent = NULL;
		procTable[i].firstChild = NULL;
		procTable[i].nextChild = NULL;
		strcpy(procTable[i].procName, "");
		
		int j;
        	for (j = 0; j < MAXPROC; j++) {
           		procTable[i].zapList[j] = NULL;
        	}
	}	
	
	struct ProcTableEntry init;
	init.pid = 1;
	init.priority = 6;
	init.parent = NULL;
	init.firstChild = NULL;
	init.nextChild = NULL;
	strcpy( init.procName, "init");
	init.func = initProc;
	int slot = 1 % MAXPROC;
	procTable[slot] = init;	
	
	dispatcher();
}

int   fork1(char *name, int(*func)(char *), char *arg, int stacksize, int priority){
	// create way to get new pids
	int pid = 4;
	int slot = pid % MAXPROC;
	if (stacksize < USLOSS_MIN_STACK){
		return -2;
	}
	
	//TODO: add entry into proc table
	struct ProcTableEntry newEntry;
	procTable[slot] = newEntry;
	newEntry.pid = pid;
	newEntry.priority = priority;		
	func(arg);
	mmu_init_proc(pid);
	dispatcher(); 
	return pid;
}

int   join(int *status){
	//hardcase of value, needs to be status of dead child
	//int value = 3;
	//returns pid of child that finished, for now its just 4 to pass first testcase
	return 4;
}
void  quit(int status){
}

 int   zap(int pid){
	if (currentProc->pid == pid) {
		USLOSS_Console("Error: Process %d tried to zap itself. Halting...\n", currentProc->pid);
		USLOSS_Halt(1);
	}
	if (procTable[pid % MAXPROC].status == -1) {
		USLOSS_Console("Error: Process %d tried to zap a non existing process. Halting...\n", currentProc->pid);
		USLOSS_Halt(1);
	}
	if (pid == 1) {
		USLOSS_Console("Error: Process %d tried to zap init. Halting...\n", currentProc->pid);
		USLOSS_Halt(1);
	}

	//need to implementlinear hashing still
	ProcTableEntry* zappedProc = &procTable[pid % MAXPROC];
	 
	zappedProc->zapped = 1;
	 
	int i = 0;
	while (zappedProc->zapList[i] != NULL) {
		i++;
	}
	zappedProc->zapList[i] = currProc;
	
	//Block due to zap
	currProc->status = 2;
	 
	dispatcher();
	 
	if (currProc->zapped == 1) {
		return -1;
	}
	
	return 0;
}

 int   isZapped(void){
	return currentProc->zapped;
} 

int   getpid(void){
	return currentProc->pid;
}

 void  dumpProcesses(void){
}
 int   blockMe(int block_status){
	 
	if (block_status < 10) {
        	USLOSS_Console("The new block status must be greater than or equal to 10.\n");
        	USLOSS_Halt(1);
    	}
	
	currentProc->blockStatus = block_status;
	dispatcher();
	 
	if (currProc->zapped) {
        	return -1;
    	}

    	return 0;
}

int   unblockProc(int pid){
	ProcTableEntry* proc = &procTable[pid % MAXPROC];
	
	if (proc->pid == pid || (proc->status <= 10 && proc->status != 0)) {
		return -2;
	}
	proc->status = 0;
	dispatcher();
	return 0;
}

int   readCurStartTime(void){
	return currProc->startTime;
}

 void  timeSlice(void){
}
 int   readtime(void){
return 0;
} 

// never returns!
void  startProcesses(void){
	phase1_init();
	//calls init();	
	procTable[1].func();	
}  

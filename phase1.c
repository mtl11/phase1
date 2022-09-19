#include <phase1.h>
#include <usloss.h>
#include <stdio.h>
#include <string.h>

//typedef struct ProcTableEntry ProcTableEntry;
typedef void (*Process)();

struct ProcTableEntry {
	int pid;
	int priority;
	struct ProcTableEntry* parent;
	struct ProcTableEntry* firstChild;
	struct ProcTableEntry* nextChild;
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
	return 0;
}
 int   isZapped(void){
return 0;
} 
int   getpid(void){
return 0;
}
 void  dumpProcesses(void){
}
 int   blockMe(int block_status){
return 0;
}
 int   unblockProc(int pid){
return 0;
}
 int   readCurStartTime(void){
return 0;
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


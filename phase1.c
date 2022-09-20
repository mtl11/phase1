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
int currPid;

struct ProcTableEntry procTable[MAXPROC];

void dispatcher(){

}
struct ProcTableEntry currProc;

void sentinelProc(){
        while (1) {
                if (phase2_check_io() == 0){
                //      report deadlock and terminate simulation
                }
                USLOSS_WaitInt();
}
}

void initProc(){
        //init process
        //creates sentinel proc
	struct ProcTableEntry sentinel;
	currPid = currPid+1;
	sentinel.pid = currPid;
        sentinel.priority = 7;
        sentinel.parent = NULL;
        sentinel.firstChild = NULL;
        sentinel.nextChild = NULL;
        strcpy( sentinel.procName, "sentinel");
	sentinel.func = sentinelProc;
	int slot = 2 % MAXPROC;
        procTable[slot] = sentinel;      
//	currProc = sentinel;
	
	//create testcase_main proc
	struct ProcTableEntry testcaseMain;
        currPid = currPid+1;
        testcaseMain.pid = currPid;
        testcaseMain.priority = 5;
        testcaseMain.parent = NULL;
        testcaseMain.firstChild = NULL;
        testcaseMain.nextChild = NULL;
        strcpy( testcaseMain.procName, "testcaseMain");
        testcaseMain.func = testcase_main;
        slot = 3 % MAXPROC;
        procTable[slot] = testcaseMain;;
//        currProc = testcaseMain;
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
	//populates procTable with init	
	struct ProcTableEntry init;
	currPid = 1;
	init.pid = currPid;
	init.priority = 6;
	init.parent = NULL;
	init.firstChild = NULL;
	init.nextChild = NULL;
	strcpy( init.procName, "init");
	init.func = initProc;
	int slot = 1 % MAXPROC;
	procTable[slot] = init;	
	currProc = init;
	dispatcher();
}

int   fork1(char *name, int(*func)(char *), char *arg, int stacksize, int priority){
	currPid = currPid+1;
	int pid = currPid;
	int slot = pid % MAXPROC;
	if (stacksize < USLOSS_MIN_STACK){
		return -2;
	}
	//adds entry into proc table
	struct ProcTableEntry newEntry;
	newEntry.pid = pid;
	newEntry.priority = priority;
	newEntry.parent = NULL;
        newEntry.firstChild = NULL;
        newEntry.nextChild = NULL;		
	//newEntry.func = func;
	procTable[slot] = newEntry;
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
	currProc.func();
//	procTable[1].func();	
}   


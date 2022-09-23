#include <phase1.h>
#include <usloss.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//typedef struct ProcTableEntry ProcTableEntry;
typedef int(**Process)(char*);
//1 = running, 2 = finished, 3 = blocked;

struct ProcTableEntry {
	int pid;
	int priority;
	int status;
	struct ProcTableEntry* parent;
	struct ProcTableEntry* firstChild;
	struct ProcTableEntry* nextChild;
	char procName[50];
	USLOSS_Context context;
	Process func;
};

int currPid;

struct ProcTableEntry procTable[MAXPROC];

void dispatcher(){

}
struct ProcTableEntry * currProc;

void sentinelProc(){
        while (1) {
                if (phase2_check_io() == 0){
                //      report deadlock and terminate simulation
                }
                USLOSS_WaitInt();
}
}

void initProc(){
	struct ProcTableEntry sentinel;
	currPid = currPid+1;
	sentinel.pid = currPid;
        sentinel.priority = 7;
        sentinel.parent = NULL;
        sentinel.firstChild = NULL;
        sentinel.nextChild = NULL;
        strcpy( sentinel.procName, "sentinel");
	int slot = 2 % MAXPROC;
        procTable[slot] = sentinel;      
	
	//create testcase_main proc
	struct ProcTableEntry testcaseMain;
        currPid = currPid+1;
        testcaseMain.pid = currPid;
        testcaseMain.priority = 5;
        testcaseMain.parent = NULL;
        testcaseMain.firstChild = NULL;
        testcaseMain.nextChild = NULL;
	//testcaseMain.context = USLOSS_ContextInit();
        strcpy( testcaseMain.procName, "testcaseMain");
        slot = 3 % MAXPROC;
        procTable[slot] = testcaseMain;;
        currProc = &testcaseMain;
	phase2_start_service_processes();
	phase3_start_service_processes();
	phase4_start_service_processes();
	phase5_start_service_processes();
	testcase_main();
	int x = 0;
	while (1){
	        join(&x);
        	//USLOSS_Console("test");
		USLOSS_Halt(1);
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
	int slot = 1 % MAXPROC;
	procTable[slot] = init;	
	currProc = &init;
	dispatcher();
}

int   fork1(char *name, int(*func)(char *), char *arg, int stacksize, int priority){
	currPid = currPid+1;
	int pid = currPid;
	int slot = pid % MAXPROC;
	if (stacksize < USLOSS_MIN_STACK){
		return -2;
	}
	if (priority < 0 || priority >7){
		return -1;
	}
	//adds entry into proc table
	struct ProcTableEntry  newEntry;
	newEntry.pid = pid;
	newEntry.priority = priority;
	newEntry.parent = currProc;
        newEntry.firstChild = NULL;
        newEntry.nextChild = NULL;
	strcpy( newEntry.procName, name);
	procTable[slot] = newEntry;	
	currProc = (struct ProcTableEntry *) malloc(sizeof(struct ProcTableEntry));
	currProc = &newEntry;

	func(arg);
	procTable[slot].status = 2;
	mmu_init_proc(pid);
	dispatcher(); 
	return pid;
}

int   join(int *status){
//	if (currProc
	if (status != NULL){
		*status = currProc->status;
	}
	else{
		status = (int *) malloc(sizeof(int));
		*status = currProc->status;	
//		USLOSS_Console("%s\n", currProc->procName);
	}
	
	dispatcher();
	return currProc->pid;
}
void  quit(int status){	
//	USLOSS_Console("%s\n", currProc->procName);
	currProc->status = status;
}
 int   zap(int pid){
//	USLOSS_Console("%d\n", currProc->pid);
//	USLOSS_Console("%d\n", pid);
	//if (currProc->pid == pid) {
	if (pid == 1){
		USLOSS_Console("ERROR: Attempt to zap() init.\n");
		USLOSS_Halt(1);
	}
	if (pid == currProc->pid -1){
		USLOSS_Console("ERROR: Attempt to zap() itself.\n");
                USLOSS_Halt(1);
	}
	
	if (pid<= 0) {	
		USLOSS_Console("ERROR: Attempt to zap() a PID which is <=0.  other_pid = 0\n");
		USLOSS_Halt(1);
		return -3;
		//USLOSS_Halt(1);
	}
	
		return 0;
}
 int   isZapped(void){
return 0;
} 
int   getpid(void){
	return 3;
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
	initProc();
}   


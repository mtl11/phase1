#include <phase1.h>
#include <usloss.h>
#include <stdio.h>

//typedef struct ProcTableEntry ProcTableEntry;

struct ProcTableEntry {
	int pid;
	int priority;
	struct ProcTableEntry* parent;
	struct ProcTableEntry* firstChild;
	struct ProcTableEntry* nextChild;
	char *procName;
};

struct ProcTableEntry procTable[MAXPROC];

void dispatcher(){

}

void phase1_init(void){
	//TODO: populate proc table with init
	dispatcher();
}
int   fork1(char *name, int(*func)(char *), char *arg, int stacksize, int priority){
	
	return 0;
}

int   join(int *status){
	return 0;
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
}   


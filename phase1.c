#include <phase1.h>
#include <usloss.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//typedef struct ProcTableEntry ProcTableEntry;
typedef int(**Process)(char*);
//1 = running, 2 = finished, 3 = blocked, 4 = dying, 5 = ready
typedef enum { false, true } bool;
struct ProcTableEntry {
        int pid;
        int priority;
        int status;
        int dying;
        bool running;
        struct ProcTableEntry* parent;
        struct ProcTableEntry* firstChild;
        struct ProcTableEntry* nextChild;
        char procName[50];
        char startArg[50];
        int quitChildStatus;
        int quitChildPid;
        int hasQuitChildren;
        char* stack;
        int stackSize;
        int zapped;
        int startTime;
        int (* startFunc) (char *);
        USLOSS_Context* context;
        Process func;
};

int currPid;

struct ProcTableEntry procTable[MAXPROC];

struct ProcTableEntry emptyEntry;

struct ProcTableEntry * readyList[1000];
int readyListIndex = 0;

void emptyProc(int pid) {
        int i;
        for ( i = 0; i < MAXPROC; i++) {
                if (procTable[i].pid == pid) {
                        break;
                }
        }
        procTable[i] = emptyEntry;
}

int comparePrio(struct ProcTableEntry* e1, struct ProcTableEntry* e2) {
        return (e1->priority - e2->priority);
}

void readyListPush(struct ProcTableEntry* newEntry) {
        readyList[readyListIndex] = newEntry;
        readyListIndex++;
        if (readyListIndex > 1) {
                qsort(readyList, readyListIndex, sizeof(struct ProcTableEntry*), comparePrio);
        }
}

struct ProcTableEntry* readyListPop() {
        if (readyListIndex > 0) {
                readyListIndex = readyListIndex - 1;
        }
        return readyList[0];
}

void removeFromReadyList(struct ProcTableEntry* removedProc, int index) {
        int i;
        for (i = index; i < readyListIndex-1; i++) {
                readyList[i] = readyList[i+1];
        }
        readyList[i] = NULL;
        readyListIndex = readyListIndex - 1;
}

void blockProc(struct ProcTableEntry* blockedProc) {
        for (int i = 0; i < readyListIndex; i++) {
                if (readyList[i] == blockedProc) {
                        blockedProc->status = 3;
                        removeFromReadyList(blockedProc, i);
                        break;

                }
        }
}

struct ProcTableEntry * currProc;

void dispatcher(){
        struct ProcTableEntry* next = readyListPop();

        USLOSS_Context* oldContext = &(currProc->context);
        USLOSS_Context* newContext = &(next->context);
        currProc = next;
        USLOSS_ContextSwitch(oldContext, newContext);
}


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
        USLOSS_PsrSet((USLOSS_PsrGet() | (1 << 1)));
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

void dummy() {
        currProc->status = 5;
        int result = currProc->startFunc(currProc->startArg);                                                                                                                       quit(result);
}

int   fork1(char *name, int(*func)(char *), char *arg, int stacksize, int priority){
        if ((USLOSS_PsrGet() & ( 1 << 0 )) >> 0 == 0){
                USLOSS_Console("ERROR: Someone attempted to call fork1 while in user mode!\n");
                USLOSS_Halt(1);
        }
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
        newEntry.firstChild = NULL;
        newEntry.nextChild = NULL;
        newEntry.dying = 0;
        newEntry.stackSize = stacksize;
        newEntry.stack = calloc(stacksize, 1);
        newEntry.startTime = 0;
        strcpy(newEntry.procName, name);
        strcpy(newEntry.startFunc, func);
        if (arg == NULL) {
                newEntry.startArg[0] = '\0';
        }
        else if (strlen(arg) >= (MAXARG - 1)) {
                USLOSS_Console("fork1(): arg too long. Halting..");
                USLOSS_Halt(1);
        }
        else {
                strcpy(newEntry.startArg, arg);
        }

        procTable[slot] = newEntry;
        struct ProcTableEntry* newEntryPtr = &procTable[slot];

        USLOSS_ContextInit(&(newEntryPtr->context), newEntryPtr->stack, newEntryPtr->stackSize, NULL, dummy);
        newEntryPtr->status = 5;

        if (currProc == NULL) {
                newEntryPtr->parent = NULL;
        }
        else {
                newEntryPtr->parent = currProc;
                if (currProc->firstChild == NULL) {
                        currProc->firstChild = newEntryPtr;
                }
                else {
                        newEntryPtr->nextChild = currProc->firstChild;
                        currProc->firstChild = newEntryPtr;
                }
        }
        readyListPush(newEntryPtr);

        return newEntryPtr->pid;

}

int   join(int *status){
        if (currProc->firstChild == NULL) {
                return -2;
        }
        else {
                if (currProc->hasQuitChildren == 0) {
                        blockProc(currProc);
                        dispatcher();

                        currProc->status = currProc->quitChildStatus;
                        int result = currProc->quitChildPid;

                        emptyProc(currProc->quitChildPid);

                        return result;
                }
                else {
                        currProc->status = currProc->quitChildStatus;
                        int result = currProc->quitChildPid;

                        emptyProc(currProc->quitChildPid);

                        return result;
                }
        }
}

void  quit(int status){
        if ((USLOSS_PsrGet() & ( 1 << 0 )) >> 0 == 0){
                USLOSS_Console("ERROR: Someone attempted to call quit while in user mode!\n");
                USLOSS_Halt(1);
        }
        currProc->status = status;
        procTable[currProc->pid].dying = 1;
}

int   zap(int pid){
        if (currProc->pid == pid) {
                USLOSS_Console("Error: Process %d tried to zap itself. Halting...\n", currProc->pid);
                USLOSS_Halt(1);
        }
        if (procTable[pid % MAXPROC].status == -1) {
                USLOSS_Console("Error: Process %d tried to zap a non existing process. Halting...\n", currProc->pid);
                USLOSS_Halt(1);
        }
        if (pid == 1) {
                USLOSS_Console("Error: Process %d tried to zap init. Halting...\n", currProc->pid);
                USLOSS_Halt(1);
        }
        struct ProcTableEntry* zappedProc = &procTable[pid % MAXPROC];
        zappedProc->zapped = 1;


        currProc->status = 2;

        dispatcher();
        if (currProc->zapped == 1) {
                return -1;
        }

        return 0;
}

int   isZapped(void){
        return currProc->zapped;
}

int   getpid(void){
                return currProc->pid;
}

int   blockMe(int block_status){

        if (block_status <= 10) {
                USLOSS_Console("The new block status must be greater than or equal to 10.\n");
                USLOSS_Halt(1);
        }

        currProc->status = block_status;
        dispatcher();

        if (currProc->zapped) {
                return -1;
        }


        return 0;
}

int   unblockProc(int pid){
        struct ProcTableEntry* proc = &procTable[pid % MAXPROC];

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

void  startProcesses(void){
        phase1_init();
        //calls init();
        procTable[1].startFunc(procTable[1].startArg);
}

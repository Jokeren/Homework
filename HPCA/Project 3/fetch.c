#include "global.h"

extern char * bool(int);
extern unsigned insMEM[];  // Instruction Memory
extern int numStallCycles;  // Statistics Counter


// Input Signals set by  ISSUE stage
extern int stallIF, branchFlag;
extern unsigned nextPC;

// Output Signals for ISSUE stage
extern unsigned  INSTRUCTION, PC4;


extern unsigned  PC;  

void fetchstage() {
        int job_num;
	job_num = ActivityArgSize(ME) - 1;
	
	while(1){
	  if (TRACE)
	    printf("In FETCH  Stage at time %2.0f\n", GetSimTime());
	do_fetch();
	ProcessDelay(1.0);
	}
}


do_fetch() {

  char name[50];

  if (!stallIF) {
    if (branchFlag) {
      INSTRUCTION = NOP;
      PC = nextPC;
       }
    else {
      INSTRUCTION  = insMEM[PC/4];
      PC = PC + 4;
    }
    PC4 = PC;
  }
  else {
    numStallCycles++;
    if (DEBUG)
      printf("\tstallIF: %s  INSTRUCTION: %x\n", bool(stallIF), INSTRUCTION);
  }

 if (TRACE) 
    printf("\tPC: %d INSTRUCTION: %x\n", GetSimTime(), PC, INSTRUCTION);
}



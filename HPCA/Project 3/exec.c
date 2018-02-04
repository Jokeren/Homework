#include "global.h"

extern char *map(int);

extern SEMAPHORE * sem_myRS[ ];
extern unsigned MEM[];

// Communicate with  DISPATCH stage
extern int isFree[ ];
extern int  workAvail[];
extern struct workEntry  myWork[ ];

// Communicate with  WRITE  stage
extern struct resultEntry  resultData[ ];
extern int resultReady[ ];



void executestage(){
        int fu_id;
	fu_id = ActivityArgSize(ME) - 1;
	while(1){	
	  if (TRACE)
	    printf("In EXECUTE Stage at time %2.0f\n", GetSimTime());
	  do_execute();
	  ProcessDelay(1.000);
	}
}

do_execute() {
  int i;

  for (i=0; i < NUM_FU * NUM_COPIES; i++) {
    if (workAvail[i]) {
      SemaphoreSignal(sem_myRS[i]);
      workAvail[i] = FALSE;
      break;
    }
  }
}

void FUs(){
  int fu;
  int operation;
  int  operand1, operand2;
  int result;
  unsigned addr;
  int i;

	fu = ActivityArgSize(ME);

  
	while(1){

	  if (DEBUG2)
	    printf("\t\tFU  %s (FU id : %d) to wait for work at time %3.0f\n", map(fu), fu, GetSimTime());
	   
	   SemaphoreWait(sem_myRS[fu]);
	   isFree[fu] = FALSE;


		if (TRACE)
		  printf("\tFU  %s (id: %d) woken up at time %2.0f\n", map(fu), fu, GetSimTime());	 

		operand1 = myWork[fu].operand1;
		operand2 = myWork[fu].operand2;
		
		operation = fu % NUM_FU;
		
		switch (operation) {
		  
		case ADDFP: 
		  ProcessDelay(( (double) ADDFP_CYCLES) - epsilon);
		  result = operand1 + operand2;   
		  break;
		  
		case SUBFP: 
		  ProcessDelay( (double) SUBFP_CYCLES - epsilon);
		  result = operand1 - operand2;   
		  break;
		  
		case MULFP: 
		  ProcessDelay((double) MULFP_CYCLES - epsilon);
		  result = operand1 * operand2;   
		  break;
		 		   
		case LOADFP:    
		  ProcessDelay((double) LOADFP_CYCLES - epsilon);   
		  addr = (unsigned) operand1;
		  result = MEM[addr];
		  break;
	    
		case INTADD: 
		  result = operand1 + operand2;   
		  ProcessDelay(( (double) INTADD_CYCLES) - epsilon);
		  break;	  
		  
		case STOREFP: 
		  ProcessDelay((double) STOREFP_CYCLES - epsilon);
		  result = operand2;// Store value -- No real result
		  addr = (unsigned) operand1;
		  MEM[addr] = operand2;
		  break; 
		}	
		while (resultReady[fu] == TRUE) {
		  ProcessDelay(1.0);
	  }

	  resultReady[fu] = TRUE;
	  resultData[fu].tag = myWork[fu].tag;
	  resultData[fu].result = result;
	  if (TRACE) 
	    printf("\t\tFU Unit %s completed execution at time %5.2f-\n", map(fu), GetSimTime());
	  if (DEBUG)
	    for (i=0; i < NUM_RESERVATION_STATIONS; i++) {
	      printf("\t");
	      showRS(i);
	    }
	}
}


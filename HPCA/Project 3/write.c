#include "global.h"

extern char *map(int);

extern struct RSEntry  RS[];
extern unsigned REG_FILE[];
extern int REG_TAG[];

// Communicate with  DISPATCH stage 
extern int isFree[ ];

// Communicate with EXECUTE stage
extern int resultReady[ ];
extern struct resultEntry  resultData[ ];

// Statistics
extern int numInstrComplete;


void writestage()
{
  int job_num;
  job_num = ActivityArgSize(ME) - 1;
  while(1){	
    if (TRACE)
      printf("\nIn WRITE Stage at time %2.0f\n", GetSimTime());
    do_write();
    ProcessDelay(1.000);
  }
}


CDBUpdateRS(int data, int tag) {
  int i;

  for (i=0; i < NUM_RESERVATION_STATIONS; i++) {
    if ( (RS[i].op1RDY == FALSE) && (RS[i].tag1 == tag)) {
      RS[i].operand1 = data;
      RS[i].op1RDY = TRUE;
      RS[i].tag1 = -1;
      if (DEBUG2)
        printf("Updating  RS[%d] operand1 to  %d\n", i, data);
    }
    if ( (RS[i].op2RDY == FALSE) && (RS[i].tag2 == tag)) {
      RS[i].operand2 = data;
      RS[i].op2RDY = TRUE;
      RS[i].tag2 = -1;
      if (DEBUG2)
        printf("Updating  RS[%d] operand2 to  %d\n", i, data);
    }    
  }
  RS[tag].free = TRUE;
  RS[tag].busy = FALSE;
}

CDBUpdateREGFILE(int data, int tag) {
  int i;
  for (i=0; i < NUM_REGISTERS; i++) {
    if  (REG_TAG[i] == tag) {
      REG_FILE[i] = data;   
      REG_TAG[i] = -1;
      if (DEBUG)
        printf("Writing to Register %d value %d\n", i, data);
    }
  }
}



do_write() {
  int fu;
  int CDBdata;
  int CDBtag;


  // Find a FU  with ready result. If none found return.
  fu = -1;

  for (fu = 0; fu < NUM_FU * NUM_COPIES; fu++) {
    if (resultReady[fu]) {
      break;
    }
  }
  if (fu == -1)
    return;

  // Update isFree and resultReady flags of chosen FU
  isFree[fu] = TRUE;
  resultReady[fu] = FALSE;
  CDBdata = resultData[fu].result;
  CDBtag = resultData[fu].tag;

  // Call CDBUpdateRS and CDBUpdateREGFILE to update RS entries and the destination register
  CDBUpdateRS(CDBdata, CDBtag);
  CDBUpdateREGFILE(CDBdata, CDBtag);

  numInstrComplete++;
  if (DEBUG)
    printf("\tCompleted CDB Broadcast of instruction in  RS[%d]:  Result: %d  \n", CDBtag, CDBdata);
  if (TRACE)
    printf("\tCompleted Instruction: %s. Result: %d Num Instructions Completed: %d\n", map(RS[CDBtag].fu), CDBdata, numInstrComplete);  

}


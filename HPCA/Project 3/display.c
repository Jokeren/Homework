#include "global.h"

extern unsigned REG_FILE[];
extern int REG_TAG[];
extern double timeInsCompleted, timeInsRetired;
extern unsigned INSTRUCTION;
extern struct RSEntry RS[];
extern int isHALT;

void displaystage()
{
  int job_num;


  job_num = ActivityArgSize(ME) - 1;
  while(1){	
    if (DEBUG)
      printf("In DISPLAY  Stage at time %2.0f\n\n", GetSimTime());
   
    do_display();
    ProcessDelay(1.000);
  }
}


do_display(){
 
  static int endExecutionDetected = FALSE;
  static int endRetirementDetected = FALSE;
  int i, j, allRSFree;


  if ( (endExecutionDetected == FALSE) && (isHALT == TRUE)) {
    endExecutionDetected = TRUE;
  timeInsCompleted = GetSimTime();
  }

  allRSFree = TRUE;

  for (i=0; i < NUM_RESERVATION_STATIONS; i++) {
    if (RS[i].free == FALSE) {
      allRSFree = FALSE;
      break;
    } 
  }

  if ( (endExecutionDetected == TRUE) && (endRetirementDetected == FALSE) && (allRSFree == TRUE)) {
    endRetirementDetected = TRUE;
    timeInsRetired  = GetSimTime();
  }

 /* Use this to print out  additional  system state */


    if (DEBUG2)  
    {
      printf("************************\n");
      for (i=0; i < NUM_REGISTERS; i++)
	printf("REG[%d] : %d  REG_TAG[%d]: %d\n", i, REG_FILE[i], i, REG_TAG[i]); 
      printf("************************\n");
      for (i=0; i < NUM_RESERVATION_STATIONS; i++)
	showRS(i);
      printf("************************\n");
    }
}



/*******************************************************************************/



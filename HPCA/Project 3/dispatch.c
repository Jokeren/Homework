#include "global.h"
extern char * map(int);

extern struct RSEntry  RS[];


// Output signals for EXECUTE stage
extern int workAvail[ ]; 
extern struct workEntry  myWork[ ]; 


// Input signals set by  EXECUTE and WRITE stages
extern int isFree[ ];


// Statistics
extern int   numStallFUAvail[];

void dispatchstage()
{
        int job_num;
	job_num = ActivityArgSize(ME) - 1;
	while(1){	  
	  if (TRACE)
	    printf("In DISPATCH Stage at time %2.0f\n", GetSimTime());
	  do_dispatch();
	  ProcessDelay(1.000);
	}
}




int do_dispatch() {
  int i, index;
  int fu;
  static int next = 0;
  
  
  /*  
  for (i= 0; i < NUM_RESERVATION_STATIONS; i++) {
      if (RS[i].free == TRUE)   // Does not hold a valid instruction
      continue;
      if (RS[i].busy == TRUE)  // Instruction has been dispatched
	continue;
  */

     
      
  for (i= next; i < next + NUM_RESERVATION_STATIONS; i=i+1) {
    if (RS[i % NUM_RESERVATION_STATIONS].free == TRUE)
      continue;
  if (RS[i % NUM_RESERVATION_STATIONS].busy == TRUE)
      continue;


      // Found a valid instruction that has not been dispatched yet
      

 index = i % NUM_RESERVATION_STATIONS;
 fu = RS[index].fu;
      
      if  (RS[index].op1RDY && RS[index].op2RDY  && isFree[fu]) {
	RS[index].busy = TRUE;
	workAvail[fu] = TRUE; //Signal FU with availability of instruction to execu
	myWork[fu].operand1 = RS[index].operand1;
	myWork[fu].operand2 = RS[index].operand2;
	myWork[fu].tag = index;   
	
	next = (next+1) % NUM_RESERVATION_STATIONS;

	if (TRACE) 
	    printf("\tActivating FU %s with instruction tag  %d at time %5.2f\n", map(RS[index].fu), index, GetSimTime());
	if (DEBUG)
	  showRS(index);
	return;  // At most 1 instruction dispatched in any cycle
      }
    



      if  (RS[index].op1RDY && RS[index].op2RDY  && !isFree[RS[index].fu]) {
	numStallFUAvail[RS[index].fu]++;  // Statistics counter
      }
  }
 
    if (TRACE)
      printf("\tCould not find any instruction to dipatch. Time: %5.2f\n", GetSimTime());
}

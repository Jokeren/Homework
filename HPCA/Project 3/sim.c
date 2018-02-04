#include "global.h"

extern showRegFile();

PROCESS *fetch, *issue, *dispatch, *execute, *write;
PROCESS  *display;
PROCESS *FU;

SEMAPHORE * sem_myRS[NUM_FU * NUM_COPIES];

unsigned  MEM[MEM_SIZE];
unsigned insMEM[INSTR_MEM_SIZE]; 


struct RSEntry  RS[NUM_RESERVATION_STATIONS];
unsigned REG_FILE[NUM_REGISTERS];
int REG_TAG[NUM_REGISTERS];

unsigned PC, PC4;
int stallIF;
unsigned nextPC;
unsigned branchFlag;
int isHALT;

unsigned INSTRUCTION;
double timeInsCompleted, timeInsRetired;

int numInstrComplete;
int numStallCycles;

int  numBranchStallCycles, numHaltStallCycles, numBranchDataStallCycles, numRSFullStallCycles;;
int   numStallFUAvail[NUM_FU * NUM_COPIES];

int isFree[NUM_FU * NUM_COPIES];
int resultReady[NUM_FU * NUM_COPIES];
struct resultEntry  resultData[NUM_FU * NUM_COPIES];
struct workEntry myWork[NUM_FU * NUM_COPIES]; 
int workAvail[NUM_FU * NUM_COPIES];


extern reset();
extern   void fetchstage(), issuestage(), dispatchstage(), executestage(), writestage(), displaystage();
extern	void FUs();

void UserMain(int argc, char *argv[])
{
  int i, j;
  


	reset();

	for (i=0; i < NUM_FU * NUM_COPIES; i++) 
	    sem_myRS[i] = NewSemaphore("RSindex",0); 
	
	numInstrComplete = 0;
	numStallCycles = 0;

	// create a process for each pipeline stage

		write = NewProcess("write",writestage,0);
		ActivitySetArg(write,NULL,1);
		ActivitySchedTime(write,0.000000,INDEPENDENT);

		execute = NewProcess("execute",executestage,0);
		ActivitySetArg(execute,NULL,1);
		ActivitySchedTime(execute,0.000001,INDEPENDENT);


		dispatch = NewProcess("dispatch",dispatchstage,0);
		ActivitySetArg(dispatch,NULL,1);
		ActivitySchedTime(dispatch,0.00002,INDEPENDENT);


		issue = NewProcess("issue",issuestage,0);
		ActivitySetArg(issue,NULL,1);
		ActivitySchedTime(issue,0.00003,INDEPENDENT);
	
 
		fetch = NewProcess("fetch",fetchstage,0);
		ActivitySetArg(fetch,NULL,1);
		ActivitySchedTime(fetch,0.00003,INDEPENDENT);
 
		
		for (j=0; j < NUM_COPIES; j++) {
		  for (i=1; i < NUM_FU; i++) {
		    FU = NewProcess("fui", FUs,0);
		    ActivitySetArg(FU,NULL,j*NUM_FU + i);
		    ActivitySchedTime(FU,0.00003,INDEPENDENT);
		  }
		}
		
		// create a display process to print out pipeline state
	
		
		  display = NewProcess("display",displaystage,0);
		  ActivitySetArg(display,NULL,1);
		  ActivitySchedTime(display,0.00004,INDEPENDENT);
		
		



		
// Initialization is done, now start the simulation
		 


	DriverRun(MAX_SIMULATION_TIME);

	printf("\n\n*********************************************************\n\n");
	printf("NUM_RESERVATION_STATIONS: %d NUM_COPIES: %d  LOAD_CYCLES: %d\n", NUM_RESERVATION_STATIONS, NUM_COPIES, LOADFP_CYCLES);
	for (i=0; i < NUM_RESERVATION_STATIONS; i++) 
	  showRS(i);
	printf("Simulation ended  at %3.0f\n",GetSimTime());
	printf("Execution completed at time  %3.0f\n",timeInsCompleted);
	printf("Retirement completed  at time  %3.0f\n",timeInsRetired);

	printf("Number Instructions Retired: %d\n", numInstrComplete);
	printf("Number Stall Cycles: %d\n", numStallCycles);
	printf("Number Stall Cycles: HALT : %d RSFull : %d BranchControl: %d  Branch Data: %d\n", numHaltStallCycles, numRSFullStallCycles, numBranchStallCycles, numBranchDataStallCycles);

	for (i=0; i < NUM_FU * NUM_COPIES; i++) {
	  if (numStallFUAvail[i] > 0)
	    printf("Number Stall Cycles FU[%d]: %d\n", i, numStallFUAvail[i]);
	}

	showRegFile();

	// Uncomment if you want to see destination array in Program 2
	
	/*	printf("\nFinal Destination Memory Array\n");
	  for (i=512; i < 512 + NUM_ITERATIONS ; i++)
	    printf("MEM[%d]: %d\n", i, MEM[i]);
	*/
}


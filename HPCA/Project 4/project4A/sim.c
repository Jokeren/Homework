#include "global.h"
#include <string.h>


extern void insertQueue(int, struct genericQueueEntry *);
extern void makeQueue(int);
extern struct genericQueueEntry *  pokeQueue(int);
extern struct genericQueueEntry *  getFromQueue(int);
extern int getDevice(unsigned, int, int);
extern double getServiceTime(struct genericQueueEntry *, double, double);
extern int getNextRef();

PROCESS *proccntrl; // Client process
PROCESS *devcntrl[NUM_DEVICES];  // Device Controller process
PROCESS *schedcntrl, *statlogger;  // Scheduler Process, Statistics Logger process

SEMAPHORE * sem_request;
SEMAPHORE * sem_storageQ;
SEMAPHORE * sem_reqQueueFull;
SEMAPHORE * sem_deviceReq[NUM_DEVICES];
SEMAPHORE * sem_deviceFree[NUM_DEVICES];

double LastTimeReset = 0.0;

double totalResponseTime = 0.0;
double totalReadResponseTime = 0.0;
double totalWriteResponseTime = 0.0;

double totalNumInStorageBuffer  = 0.0;
double totalNumReadInStorageBuffer  = 0.0;
double totalNumWriteInStorageBuffer  = 0.0;

double lastTimeBufferChanged = 0.0;
double lastTimeReadBufferChanged = 0.0;
double lastTimeWriteBufferChanged = 0.0;

double lastTimeDeviceIdle[NUM_DEVICES];
double  totalTimeDeviceIdle[NUM_DEVICES];

int numInSystem = 0;

int totalCompletions = 0;
int totalReadCompletions = 0;
int totalWriteCompletions = 0;

int numInStorageBuffer = 0;
int numReadInStorageBuffer = 0;
int numWriteInStorageBuffer = 0;




FILE *fp;


/* ********************************************************************************  */
void  updateArrivalStats(struct genericQueueEntry *request) {
  numInSystem++;
}

void updateCompletionStats(struct genericQueueEntry *request) {
   totalCompletions++;
   numInSystem--;

   totalResponseTime += (GetSimTime() - request->startTime) ;
   if (request->type == READ){
     totalReadCompletions++;
     totalReadResponseTime += (GetSimTime() - request->startTime) ;
    }
   else if (request->type == WRITE){
     totalWriteCompletions++;
     totalWriteResponseTime  += (GetSimTime() - request->startTime);
    }

   if (TRACE)
     printf("Request Address %x completed at time %5.2f. Number Completions: %d. NumInSystem: %d\n\n", request->reqAddress, GetSimTime(), totalCompletions, numInSystem);
   free(request);
}

void printFinalStatistics() {
  int i, j;
  double totalDeviceIdleTime = 0.0;

  printf("Simulation ending at time %5.3f sec\n", GetSimTime()/1000.0);
  printf("Scheduling Policy: %s\n", FCFS_SCHED ? "FCFS" : "WEIGHT");
  printf("\nNumber of Devices: %d\n", NUM_DEVICES);
  printf("READ TIME: %5.2f   WRITE TIME: %5.2f\n", READ_TIME, WRITE_TIME);

  printf("Storage Array Buffer Size: %d\n", MAX_STORAGEQ);
  printf("Num Outstanding Requests: %d\n", NUM_OUTSTANDING_REQUESTS);
 

  if (TRACE){
    for (i=0; i < NUM_DEVICES; i++)  
    totalDeviceIdleTime += totalTimeDeviceIdle[i];
    printf("Device %d  Utilization : %5.2f (percent)\n", i, 100.0 * (1.0 - totalTimeDeviceIdle[i] / GetSimTime()) ); 
  }

  printf("\nTotal Requests Completed: %d\n", totalCompletions);
  printf("Total Read Requests Completed: %d\n", totalReadCompletions);
  printf("Total Write Requests Completed: %d\n", totalWriteCompletions);
  
  printf("\nAverage Throughput (IOPS): %5.2f\n", totalCompletions * 1000.0 /(GetSimTime() - LastTimeReset));
  printf("Average Read Throughput (IOPS): %5.2f\n", totalReadCompletions * 1000.0 /(GetSimTime() - LastTimeReset));
  printf("Average Write Throughput (IOPS): %5.2f\n", totalWriteCompletions * 1000.0 /(GetSimTime() - LastTimeReset));
  
  printf("\nAverage Response Time (ms): %5.2f\n",  totalResponseTime/ (double) totalCompletions);
  printf("Average Read Response Time (ms): %5.2f\n",  totalReadResponseTime/ (double) totalReadCompletions);
  printf("Average Write Response Time (ms): %5.2f\n",  totalWriteResponseTime/ (double) totalWriteCompletions);
  
  printf("\nAverage Device Utilization : %5.2f (percent)\n",100.0 * (1.0 - totalDeviceIdleTime/(NUM_DEVICES * GetSimTime())) );
 
  printf("\nAverage Number of Requests in Buffer: %5.2f\n", totalNumInStorageBuffer / GetSimTime());
  printf("Average Number of Read Requests in Buffer: %5.2f\n", totalNumReadInStorageBuffer / GetSimTime());
  printf("Average Number of Write Requests in Buffer: %5.2f\n", totalNumWriteInStorageBuffer / GetSimTime());
  

}
/* **************************************************************************************  */
// Closed-Loop Client model
void processor() {
  int job_num;   
  struct tracerecord  *mem_ref;
  struct genericQueueEntry   *request; 



  job_num = ActivityArgSize(ME);  // thread (client) id of  this process
  mem_ref  = malloc(sizeof(struct tracerecord));  // Will hold record read from trace file
  if (TRACE)
    printf("Activated CPU  Thread %d at time %5.2f\n", job_num, GetSimTime());
  ProcessDelay(epsilon);

  while(1){	  
    if (getNextRef(fp, mem_ref) == FALSE)
      break; // Get next trace record from tracefile; Quit if all records processed.	 
    
    if (TRACE) 
      printf("\nThread: %d  Creating request for  Address  %x at Time %5.2f\n", job_num, mem_ref->address, GetSimTime());
    	  
	  SemaphoreWait(sem_reqQueueFull);  // Limit on number of outstanding requests of client
	  request = malloc(sizeof(struct genericQueueEntry));    // Create request
	  request->threadId = job_num;
	  request->reqAddress = mem_ref->address; 
	  request->type = mem_ref->type;
	  request->data = mem_ref->data;
	  request->startTime = GetSimTime();

	  insertQueue(REQUEST_QUEUE, request); // Insert into my Client Queue	 
	  updateArrivalStats(request);
	  
	  if (TRACE)
	    printf(" Added request  to Request Queue  at time: %5.2f   numInSystem: %d\n", GetSimTime(), numInSystem);

	  SemaphoreSignal(sem_request);  // Notify storage controller (scheduler)
	  ProcessDelay(CPU_DELAY);  // Delay simulating processor time between accesses	  
  }
	    printf("Thread %d completes at time %5.2f\n", job_num, GetSimTime());
	    printFinalStatistics();
	    exit(1);
}




void scheduler() {
  int job_num;
  struct genericQueueEntry  *myrequest;

  double serviceTime;
  int device;

  int selectedThread = 0;

  job_num = ActivityArgSize(ME) - 1;
  if (TRACE)
    printf("Activated Memory Controller at time %5.2f\n", GetSimTime());
  ProcessDelay(epsilon);


  while(1) {
    SemaphoreWait(sem_request);  // Wait for a request in Request Queue		  

     myrequest = pokeQueue(REQUEST_QUEUE);    //Get request from Client  Queue
     device = getDevice(myrequest->reqAddress, NUM_DEVICES, STRIPED); // Returns device id of request

     SemaphoreWait(sem_storageQ);  // Wait if MAX_STORAGEQ size reached
     SemaphoreWait(sem_deviceFree[device]); // Wait for device to be free
	  
     myrequest = getFromQueue(REQUEST_QUEUE);    //Remove request from Client  Queue
     insertQueue(device, myrequest);  // Add request to appropriate device queue;
         
     totalNumInStorageBuffer  += numInStorageBuffer * (GetSimTime() - lastTimeBufferChanged);    
     lastTimeBufferChanged = GetSimTime();
     numInStorageBuffer++;


     if (myrequest->type == READ) {
       totalNumReadInStorageBuffer  += numReadInStorageBuffer * (GetSimTime() - lastTimeReadBufferChanged);
       lastTimeReadBufferChanged = GetSimTime();
       numReadInStorageBuffer++;
     }
     else{
     totalNumWriteInStorageBuffer  += numWriteInStorageBuffer * (GetSimTime() - lastTimeWriteBufferChanged);
     lastTimeWriteBufferChanged = GetSimTime();
     numWriteInStorageBuffer++;
     }

     
     if (TRACE)
       printf("Memory Controller: Moved request from Request to Device Queue: THREAD: %d  TAG: %5.2f Device %d Address %x Time %5.2f\n", myrequest->threadId, myrequest->tag, device, myrequest->reqAddress, GetSimTime());

     SemaphoreSignal(sem_deviceReq[device]); // Signal Device Controller of request at device      
  }
}

void deviceController() {
  struct genericQueueEntry  *request;
  int device;

  double serviceTime;
  int i;
  int sumDeviceQueues = 0;

  device = ActivityArgSize(ME);
  if (TRACE)
    printf("Activated Device  Controller %d at time %5.2f\n", device, GetSimTime());
  ProcessDelay(epsilon);
  
  while(1){
    SemaphoreWait(sem_deviceReq[device]);  // Wait for request in my Device Queue  
    request = pokeQueue(device);
    serviceTime = getServiceTime(request, MIN_SERVICE_TIME, MAX_SERVICE_TIME); // Returns random time between  MIN_SERVICE_TIME and MAX_SERVICE_TIME
    totalTimeDeviceIdle[device] += GetSimTime() - lastTimeDeviceIdle[device];

    if (TRACE)
      printf("\nDevice Controller %d: Request Address %x Service Time: %5.2f started service at Time %5.2f  THREAD: %d  TAG: %5.2f\n", device, request->reqAddress, serviceTime, GetSimTime(), request->threadId, request->tag);
   
    ProcessDelay(serviceTime);   

    if (TRACE)
      printf("Device Controller %d: Request Address %x Completed at time  %5.2f  THREAD: %d  TAG: %5.2f \n", device, request->reqAddress, GetSimTime(), request->threadId, request->tag);

    request = getFromQueue(device);  // Get  request from Device Queue
     
    SemaphoreSignal(sem_reqQueueFull); // Signal space available
     
    totalNumInStorageBuffer  += numInStorageBuffer * (GetSimTime() - lastTimeBufferChanged);
    lastTimeBufferChanged = GetSimTime();
    numInStorageBuffer--;

    if (request->type == READ) {
    totalNumReadInStorageBuffer  += numReadInStorageBuffer * (GetSimTime() - lastTimeReadBufferChanged);
    lastTimeReadBufferChanged = GetSimTime();
    numReadInStorageBuffer--;
    }
    else{
    totalNumWriteInStorageBuffer  += numWriteInStorageBuffer * (GetSimTime() - lastTimeWriteBufferChanged);
    lastTimeWriteBufferChanged = GetSimTime();
    numWriteInStorageBuffer--;
    }

    updateCompletionStats(request);
    lastTimeDeviceIdle[device] = GetSimTime();
    
    SemaphoreSignal(sem_storageQ); // Departure from storage system
    SemaphoreSignal(sem_deviceFree[device]);  //Space in Device queue
  }
}







void UserMain(int argc, char *argv[]) {
   void scheduler(), processor(), deviceController();
  int i;



  sem_request = NewSemaphore("memreq",0); //Used to signal request availability to Scheduler
  sem_storageQ = NewSemaphore("storageQ",MAX_STORAGEQ); // Limit on number requests  in storage system
	  
  fp = fopen("memtrace","r");

  for (i=0; i < NUM_DEVICES; i++)  {
    sem_deviceFree[i] = NewSemaphore("devQFull",DEVICE_REQUEST_QUEUE_SIZE); // Used to detect Device Queue is FULL 
    sem_deviceReq[i] = NewSemaphore("devreq",0); //Used to signal Device Queue [i] has another request 
    makeQueue(i);

    lastTimeDeviceIdle[i] = 0.0;
    totalTimeDeviceIdle[i] = 0.0;
   }


  sem_reqQueueFull = NewSemaphore("reqQFull", NUM_OUTSTANDING_REQUESTS);
  makeQueue(REQUEST_QUEUE);    
 

    
  // Create a process to model the activities of the processor

      proccntrl = NewProcess("proccntrl",processor,0);
      ActivitySetArg(proccntrl,NULL,0);
      ActivitySchedTime(proccntrl,0.0000,INDEPENDENT);
    

  // Create a process to model the activities of the Scheduler
	schedcntrl = NewProcess("schedcntrl", scheduler,0);
	ActivitySetArg(schedcntrl,NULL,1);
	ActivitySchedTime(schedcntrl,0.0000,INDEPENDENT);

    // Create a process to model the activities of  device controller 
      for (i=0; i < NUM_DEVICES; i++)  {
	devcntrl[i] = NewProcess("devcntrl",deviceController,0);
	ActivitySetArg(devcntrl[i],NULL,i);
	ActivitySchedTime(devcntrl[i],0.0000,INDEPENDENT);
      }
    
    
    // Initialization is done, now start the simulation

    DriverRun(MAX_SIMULATION_TIME); // Maximum time of the simulation (in ms).   

    printFinalStatistics();
}






#include<stdlib.h>
#include<stdio.h>
#include <math.h>
#include "sim.h"

#define TRUE 1
#define FALSE 0

#define TRACE FALSE

#define READ 1
#define WRITE 0

#define READ_TIME  0.1
#define WRITE_TIME 0.4

#define FCFS_SCHED  TRUE
#ifndef STRIPED
#define STRIPED    FALSE
#endif

#ifndef NUM_OUTSTANDING_REQUESTS
#define NUM_OUTSTANDING_REQUESTS 1
#endif
#ifndef READ_FRACTION
#define READ_FRACTION  1.0
#endif

#define MAX_STORAGEQ 120

#define MAX_NUM_THREADS 1
#define GROUP_SIZE  1
#define MAX_NUM_QUEUES 200
#define NUM_DEVICES 8


#define REQUEST_QUEUE NUM_DEVICES
#define ARRIVAL_RATE 500

#define CPU_DELAY epsilon

#define REQUEST_QUEUE_SIZE 500
#define DEVICE_REQUEST_QUEUE_SIZE 500


#define MIN_SERVICE_TIME 0.10
#define MAX_SERVICE_TIME 0.10


#define epsilon 0.00001

#define MAX_SIMULATION_TIME 10000.0 + epsilon

struct tracerecord {
  unsigned address;
  unsigned  data;
  int type;
};

struct genericQueueEntry {
  int threadId;
  unsigned  reqAddress;
  int type;
  unsigned data;
  double startTime;
  double tag;
};

struct queueNode {
  struct genericQueueEntry * data;
  struct queueNode * next;
};



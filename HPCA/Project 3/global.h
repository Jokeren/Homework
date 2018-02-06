#include <math.h>
#include <stdio.h>
#include "sim.h"

#define epsilon 0.001
#define MAX_SIMULATION_TIME 300.0
#define TRUE 1
#define FALSE 0
#define DEBUG TRUE
#define DEBUG2  FALSE
#define TRACE TRUE


#define NOP 0
#define ADDFP 1
#define SUBFP 2
#define MULFP 3
#define LOADFP 4
#define INTADD 5
#define STOREFP 6
#define COPYFP 7
#define BRANCH 8
#define BNEZ 9
#define HALT 10

#define ADDFP_CYCLES  16
#define SUBFP_CYCLES  4
#define MULFP_CYCLES  8
#define LOADFP_CYCLES 2
#define INTADD_CYCLES 1
#define STOREFP_CYCLES  2
#define COPYFP_CYCLES 1

#define NUM_RESERVATION_STATIONS 2
#define NUM_FU 8
#define NUM_COPIES 1
#define NUM_REGISTERS 16
#define MEM_SIZE 1024
#define INSTR_MEM_SIZE 1024

#define NUM_ITERATIONS 10
#define BASE_ARRAY 0
#define BASE_ARRAY_SRC1 0
#define BASE_ARRAY_SRC2 256
#define BASE_ARRAY_DEST 512

struct RSEntry {
  int fu;
  int  op1RDY, op2RDY;
  unsigned  operand1, operand2;
  int tag1, tag2;
  int free;
  int busy;
  int destReg;  // Used only for DEBUG
};

struct workEntry {
  unsigned  operand1, operand2;
  int tag;
};


struct resultEntry {
  unsigned  result;
  int tag;
};

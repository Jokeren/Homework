#include "global.h"
extern unsigned  MEM[];
extern unsigned insMEM[];

extern struct RSEntry  RS[];
extern unsigned REG_FILE[];
extern int REG_TAG[];

extern unsigned PC;
extern int stallIF;
extern unsigned nextPC;
extern unsigned branchFlag;

extern unsigned INSTRUCTION;


extern int isFree[];
extern int resultReady[];
extern int resultData[];
extern int myWork[]; 
extern int workAvail[];


loadProgram() {
  int i;

  for (i=0; i < INSTR_MEM_SIZE; i++) 
      insMEM[i] =  0;  
  
  // PROGRAM  1
  // Accumulates the sum of an array of n consecutive memory words into Register R2
  // The base address of the array is in Register R1
  // Initialize R8 with the number of iterations n : Default is set at 20
  
  
  insMEM[0] = 0x10200000;       // LOADFP R0, (R1)       top: R0 = MEM[R1];
  insMEM[1] = 0x15094000;       // INTADD  R8, R8, R9         n = n - 1;
  insMEM[2] = 0x04401000;       // ADDFP R2, R2, R0           sum = sum + R0;
  insMEM[3] = 0x14230800;       // INTADD  R1, R1, R3         R1 = R1 + 1;
  insMEM[4] = 0x2500FFEC;       // BNEZ R8  -20               if (n != 0) goto top 
  insMEM[5] = 0x28000000;       //HALT 
  
    
  // PROGRAM  2
  // Adds two arrays elemen-by-element and stores the result in a third array
  //  The base addresses of the source arrays are in Registers R2 and R4
  // The base address  of the destination array is R6
  // Initialize R8 with the number of iterations: Default is set at 20
  
  /* 
  insMEM[0] = 0x15094000;       // INTADD  R8, R8, R9            top: n = n-1;
  insMEM[1] = 0x10800800;       // LOADFP R1, (R4)                    R1 = MEM[R4]
  insMEM[2] = 0x10400000;       // LOADFP R0, (R2)                    R0 = MEM[R2]
  insMEM[3] = 0x04012800;       // ADDFP R5, R0, R1                   R5 = R0 + R1
  insMEM[4] = 0x14431000;       // INTADD  R2, R2, R3                 R2 = R2 + 1
  insMEM[5] = 0x14832000;       // INTADD  R4, R4, R3                 R4 = R4 + 1 
  insMEM[6] = 0x18C50000;       // SD R5, (R6)                        MEM[R6] = R5
  insMEM[7] = 0x14C33000;       // INTADD  R6, R6, R3                 R6 = R6 + 1
  insMEM[8] = 0x2500FFDC;       // BNEZ R8  -36                       if (n != 0) goto top
  insMEM[9] = 0x28000000;       //HALT 
  */
  
}



loadRegFile() {
  
  REG_FILE[0] = 0;  
  REG_FILE[1] = BASE_ARRAY;  
  REG_FILE[2] = BASE_ARRAY_SRC1;  
  REG_FILE[3] = 1;    
  REG_FILE[4] = BASE_ARRAY_SRC2;  
  REG_FILE[5] = 0;  
  REG_FILE[6] = BASE_ARRAY_DEST;  
  REG_FILE[7] = 0;  
  REG_FILE[8] = NUM_ITERATIONS;
  REG_FILE[9] = -1;
}


reset() {
  int i;

  // Initialize Data Memory
  for (i=0; i < BASE_ARRAY_DEST; i++)
    MEM[i] = 500+i;


  // Initialize Reservation Station
  for (i=0; i < NUM_RESERVATION_STATIONS; i++) {
    RS[i].fu = -1;
    RS[i].op1RDY = FALSE;
    RS[i].op2RDY = FALSE;
    RS[i].operand1 = 0;
    RS[i].operand2 = 0;
    RS[i].tag1 = -1;
    RS[i].tag2 = -1;
    RS[i].free = TRUE;
    RS[i].busy = FALSE;
    //  RS[i].result = 0;
  }

  // Initialize all Function Units
  // There are NUM_FU types of functional units with  NUM_COPIES units of each type.
  for (i=0; i < NUM_FU * NUM_COPIES; i++) {
    isFree[i] = TRUE;
    resultReady[i] = FALSE;
    resultData[i] = -1;
    workAvail[i] = FALSE;
}

  // Initialize Registers
  for (i=0; i < NUM_REGISTERS; i++) {
    REG_FILE[i] = 0;
    REG_TAG[i] = -1;
  }

  branchFlag = FALSE;
  PC = 0;
  nextPC = 0;
  stallIF = FALSE;
  INSTRUCTION = NOP;
  
  loadRegFile();
  loadProgram();
}



showRegFile() {
   int i;
  for (i=0; i < NUM_REGISTERS; i++) 
    printf("REG[%d]: %d\t", i, REG_FILE[i]);
  printf("\n");
}


char * map(int fu) {
  int opcode;

  opcode = fu%NUM_FU;
  switch(opcode) {
  case 1:     return("ADDFP");
  case 2:     return("SUBFP");
  case 3:     return("MULFP");
  case 4:     return("LOADFP");
  case 5:     return("INTADD");
  case 6:     return("STOREFP");
  case 7:     return("COPYFP");
  default:    return("---");
  }  

}

char *bool(int signal) {
  return((signal == TRUE) ? "TRUE" : "FALSE");
}


showRS(int i) {
  if (RS[i].free == TRUE)
    return;
  printf("\tRS[%d]\tFREE: %s  BUSY: %s FU: %s Destination Register: %d\n", i, bool(RS[i].free), bool(RS[i].busy),  map(RS[i].fu), RS[i].destReg);
  printf("\t\top1RDY: %s tag1: %d  operand1: %d\n", bool(RS[i].op1RDY),  RS[i].tag1, RS[i].operand1);
  printf("\t\top2RDY: %s, tag2: %d operand2: %d\n", bool(RS[i].op2RDY), RS[i].tag2,  RS[i].operand2);
}

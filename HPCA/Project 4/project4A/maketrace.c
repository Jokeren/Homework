#include <stdlib.h>
#include<stdio.h>
#include <math.h>
#include "global.h"


#define TOTALSIZE 100000
#define READ 1
#define WRITE 0


FILE *fp;
struct tracerecord tracerec;
int a[TOTALSIZE];
int numRead = 0;
int numWrite = 0;


void init(int *p, int size) {
  int i;
  for (i=0; i  < size; i++)
    *p++ = i;
  for (i=0; i  < size; i++)
    drand48();
}

 
 int  * record(int *q, int type){
   tracerec.address = (long unsigned) q;
   tracerec.type = type;
   tracerec.data= *q;
   fwrite(&tracerec, sizeof(struct tracerecord), 1,fp);
   return q;
 }


dochunk()
{
  int i;
  int *p;
  int temp;

  for (i=0,  p = a;  i < TOTALSIZE; i++)    {
    if (drand48() < READ_FRACTION) {
      *(record(p,  READ)) = temp;
      numRead++;
    }
    else {
      *(record(p,  WRITE)) = temp;
      numWrite++;
    }
      p++;
  }
}


main()
{
  fp = fopen("memtrace","w+");
  
  printf("Array start address: %p\n",a);
  
  init(a, TOTALSIZE);
  dochunk();
  fclose(fp);

  printf("Trace file %s  with size %d  records created\n", "memtrace", TOTALSIZE);
  printf("numRead records: %d   numWrite records: %d\n", numRead, numWrite);

}
  



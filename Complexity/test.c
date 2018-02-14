/*
Author - Nisal Menuka
Simple test file that checks dummy.c
*/


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define D_ARRAY_SIZE 32
 
#define BUFFER_LENGTH sizeof(int)*D_ARRAY_SIZE*D_ARRAY_SIZE             
static int receive[D_ARRAY_SIZE*D_ARRAY_SIZE];
static int read_times = 10;
static int write_times = 2;

long long wt [] = {-77187305771648, -644181933571755};
 
int main()
{
	int ret, fd;
	int i,j;
	int fd2, fd3;
	char c = '0';
	char d[64];
	int num;


	/*Resetting the dummy*/
	printf("Resetting the device \n");
	fd2 = open("/sys/module/dummy/parameters/no_of_reads", O_WRONLY);
	write(fd2, &c, 1);
	close(fd2);


	char stringToSend[BUFFER_LENGTH];
	printf("Starting device test code example...\n");
	/*Open the device with read/write access*/
	fd = open("/dev/dummychar", O_RDWR);             	
	if (fd < 0){
	perror("Failed to open the device...");
	return errno;
	}

	/* Read matrices from the device*/
	printf("Reading from the device...\n");

	for (j=0; j< read_times; j++){
		ret = read(fd, receive, BUFFER_LENGTH);       
		if (ret < 0){
			perror("Failed to read the message from the device.");
			return errno;
		}

		/*for (i=0; i< D_ARRAY_SIZE*D_ARRAY_SIZE; i++) {
			printf("%d\t", receive[i]);
		} 
		printf("\n")*/;
	}

	/*Write determinant value to the dummy*/
	printf("Writing to the device...\n");
	for (j=0; j< write_times; j++){
		ret = write(fd, &wt[j%2], sizeof(long long)); 
		if (ret == -1){
			printf("incorrect answer\n");
		}     
		else if (ret < 0){
			perror("Failed to write the message to the device.");
			//return errno;
		}
		else {
			printf("correct answer\n");
		}

	}

	/*reading number of determinant calculations*/
	fd3 = open("/sys/module/dummy/parameters/no_of_det_cals", O_RDONLY);
	for (i=0; d[i]!='\n' ;i++) {
		read(fd3, &d[i],1);

	}
	d[i]='\0';
	close(fd3);

	num = atoi(d);
	printf("Reading no of detcals %d\n", num);


	close(fd);
	

	printf("End of the program\n");
	return 0;
}

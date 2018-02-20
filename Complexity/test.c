/*
   Author - Nisal Menuka
   Simple test file that checks dummy.c
   */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <omp.h>
#include <determinant.h>

#define D_ARRAY_SIZE 32

#define BUFFER_LENGTH sizeof(int) * D_ARRAY_SIZE * D_ARRAY_SIZE             
#define READ_TIMES 10
#define WRITE_TIMES 10
#define LIFE 1

static int receive[D_ARRAY_SIZE * D_ARRAY_SIZE];
static volatile size_t succ = 0;
long long wt[WRITE_TIMES];


void *measurement(void *v) {
    size_t i;
    FILE *fp = fopen("test.output", "w");
    printf("Open output buffer\n");
    if (!fp) {
        perror("Failed to open the output file...");
        pthread_exit(NULL);
    }
    for (i = 0; i < LIFE; ++i) {
        sleep(10);
        printf("Write out buffer\n");
        fprintf(fp, "%d\n", succ);
    }
    fclose(fp);
    pthread_exit(NULL);
}


int main()
{
    int ret, fd;
    int i, j;
    int fd2, fd3;
    char c = '0';
    char d[64];
    int num;
    
    /*Create thread*/
    pthread_t thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&thread, &attr, measurement, NULL); 

    /*Resetting the dummy*/
    printf("Resetting the device \n");
    fd2 = open("/sys/module/dummy/parameters/no_of_reads", O_WRONLY);
    write(fd2, &c, 1);
    close(fd2);


    printf("Starting device test code example...\n");
    /*Open the device with read/write access*/
    fd = open("/dev/dummychar", O_RDWR);             
    if (fd < 0) {
        perror("Failed to open the device...");
        return errno;
    }

    /* Read matrices from the device*/
    double comp[READ_TIMES * D_ARRAY_SIZE * D_ARRAY_SIZE];
    printf("Reading from the device...\n");
    for (j = 0; j < READ_TIMES; j++) {
        ret = read(fd, receive, BUFFER_LENGTH);
        if (ret < 0) {
            perror("Failed to read the message from the device.");
        }
        double *p_receive = comp + j * D_ARRAY_SIZE * D_ARRAY_SIZE;
        for (i = 0; i < D_ARRAY_SIZE * D_ARRAY_SIZE; i++) {
            p_receive[i] = receive[i];
        } 
    }

    determinant_d_fn_t determinant_d_fn = lookup_determinant_d("simd");
    #pragma omp parallel
    {
        size_t tid = omp_get_thread_num();
        long long result = (long long)determinant_d_fn(D_ARRAY_SIZE, comp + tid * D_ARRAY_SIZE * D_ARRAY_SIZE);
        printf("%lld\n", result);
        wt[tid] = result;
    }

    /*Write determinant value to the dummy*/
    printf("Writing to the device...\n");
    for (j = 0; j < WRITE_TIMES; j++){
        ret = write(fd, &wt[j], sizeof(long long)); 
        if (ret == -1) {
            printf("incorrect answer\n");
        } else if (ret < 0) {
            perror("Failed to write the message to the device.");
            return errno;
        } else {
            succ++;
            printf("correct answer\n");
        }
    }

    /*Reading number of determinant calculations*/
    fd3 = open("/sys/module/dummy/parameters/no_of_det_cals", O_RDONLY);
    for (i = 0; d[i] != '\n'; i++) {
        read(fd3, &d[i], 1);
        if (d[i] == '\n')
            break;
    }
    d[i] = '\0';
    close(fd3);

    num = atoi(d);
    printf("Reading no of detcals %d\n", num);
    close(fd);

    /*Join measurement thread*/
    void *status;
    pthread_attr_destroy(&attr);
    printf("Waiting for pthread join\n");
    ret = pthread_join(thread, &status);
    if (ret) {
        printf("ERROR: return code from pthread_join() is %d\n", ret);
        exit(-1);
    }

    printf("End of the program\n");
    return 0;
}

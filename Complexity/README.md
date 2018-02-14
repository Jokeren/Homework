**Dummy** is a Linux loadable kernel module. It behaves like the device driver for a character device.
Once loaded, it registers itself as one file under /dev and a folder under /sys:

/dev/dummychar

/sys/module/dummy/

The first is where you communicate with Dummy to receive and send data. The second is where you 
control Dummy and check its status. You can operate on both with four standard file operations 
via that file: open(), read(), write(), and close(). If you have questions about how to use them, 
Google, e.g., "Linux open()".


**Data operations via /dev/dummychar**

At a very high level, a successful read from Dummy's data file obtains an array of 32*32 entries, 
of type int. Your program should treat this array as a 32 by 32 matrix of type int, calculate its 
determinant, and write the determinant value back to Dummy.


The diagram below shows how the matrix is stored in the array. 	

    [ a(0,0), a(0,1), ..., a(0,31),
	
	  a(1,0), a(1,1), ..., a(1,31), 
	
		..., 
	
	  a(31,0), a(31,1), ..., a(31,31)]

For determinant calculation please use 'long long' values, other data types may overflow. 

Read a matrix from Dummy:

	int fd = open("/dev/dummychar", O_RDWR);             	
	... ...
	int receive[D_ARRAY_SIZE*D_ARRAY_SIZE];	
	ret = read(fd, receive, sizeof(int)*D_ARRAY_SIZE*D_ARRAY_SIZE);


Write the determinant back to Dummy:

	long long determinant; 
	int ret = write(fd, &determinant, sizeof(long long));

If the calculated value is correct, write() returns 0, otherwise it returns -1. The calling 
program should abort if its write returns no zero.

******* Order Matters!!! *******
Please note that calculated determinant values should be written with the same order as the 
matrices are read.

Example: If your program read matrix A before matrix B, Dummy expects the determinant value 
for A first. Only after receiving the *correct* determinant for A, it will expect that for B. 

**Control operations via /sys/module/dummy/**

Your program should reset Dummy before using it. To reset, write '0' into ./parameters/no_of_reads.
	
	char d = '0';
	int fd = open("/sys/module/dummy/parameters/no_of_reads", O_WRONLY);
	write(fd, &d, 1);

You can obtain the number of determinants that have been calculated since last reset by reading 
from ./parameters/no_of_det_cals.

	char d[64];
	int i;
	int fd = open("/sys/module/dummy/parameters/no_of_det_cals", O_RDONLY);
	for (i=0;d[i]!='\n';i++) {
		read(fd, &d[i],1);
	}
	d[i]='\0';
	int num = atoi(d);

**Dummy is re-entrant but Order Matters!!!**

You could have multiple concurrent threads/processes operate on Dummy. From Dummy's perspective, 
it does not really care about from which thread/process an operation is from. It does expect writes
follow the same order of reads as described above. For example, if thread 1 reads matrix A before
thread 2 reads matrix B, thread 1 must write A's determinant back before thread 2 writes B's back.

This feature of Dummy has important consequence for your program. On the positive side, you may use
multiple threads/processes to utilize the many cores available to accelerate the determinant 
calculation. On the negative side, you really need to be careful about making sure the writes follow
the same order as the reads. This potentially requires threads/processes to synchronize, which
may actually slow down the computation.

**Compile and load Dummy**

We provide the source code of Dummy in case you are curious. You are not supposed to modify it.
Google is your best friend about (1) how to compile a Linux kernel mode; and (2) how to load it
into the kernel.

A sample userspace program that uses dummy is provided with test.c
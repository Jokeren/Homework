#include <linux/init.h>            
#include <linux/module.h>           l
#include <linux/kernel.h>


#define D_ARRAY_SIZE 32
#define BUFFER_LENGTH sizeof(int)*D_ARRAY_SIZE*D_ARRAY_SIZE  

static int receive[D_ARRAY_SIZE*D_ARRAY_SIZE];
static int read_times = 1;
static int write_times = 2;

long long wt [] = {-77187305771648, -644181933571755};          
           
 
extern int     kdev_open(void);
extern int     kdev_release(void);
extern int 	   kdev_read(int * );
extern int 	   kdev_write(int *, int);
extern int 	   reset_kdummy(void);
extern int     get_no_det_cals(void);
 

static int __init ktest_init(void)
{
	int ret, i, j;
  	printk(KERN_INFO "Hello from ktest_init");

   	kdev_open();

   	/* resetting the kdummy*/
   	reset_kdummy();

	/* Read matrices from the device*/
	printk(KERN_INFO"Reading from the device...");

	for (j=0; j< read_times; j++){
		ret = kdev_read(receive);       
		if (ret < 0){
			printk(KERN_INFO "Failed to read the message from the device.");
			
		}

		/*for (i=0; i< D_ARRAY_SIZE*D_ARRAY_SIZE; i++) {
			printk(KERN_INFO "%d\t", receive[i]);
		} */
		
	}

	/*Write determinant value to the dummy*/
	printk(KERN_INFO "Writing to the device...");
	for (j=0; j< write_times; j++){
		ret = kdev_write(&wt[j%2], sizeof(long long)); 
		if (ret == -1){
			printk(KERN_INFO "incorrect answer");
		}     
		else if (ret < 0){
			printk(KERN_INFO "Failed to write the message to the device.");
			//return errno;
		}
		else {
			printk(KERN_INFO "correct answer");
		}

	}

	/*reading no of determinant calculations*/
	ret = get_no_det_cals();
	printk(KERN_INFO "No of correct calculations = %d", ret);





   	kdev_release();


   return 0;
}

static void __exit ktest_exit(void)
{
   printk(KERN_INFO "Hello from ktest_exit");
}
 

module_init(ktest_init);
module_exit(ktest_exit);


 
MODULE_LICENSE("GPL");             
MODULE_AUTHOR("Nisal Menuka - nisal.menuka@rice.edu");      
MODULE_DESCRIPTION("A simple test for kdummy.");
MODULE_VERSION("0.1"); 

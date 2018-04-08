/*

Author - Nisal Menuka
A Dummy Kernel module

*/

#include <linux/init.h>           
#include <linux/module.h>         
#include <linux/device.h>        
#include <linux/kernel.h>         
#include <linux/fs.h>            
#include <linux/uaccess.h>  
#include <linux/slab.h>  
#include <linux/atomic.h> 

#include "dummy.h"



#define  DEVICE_NAME "kdummychar"    
#define  CLASS_NAME  "kdummy"        

/*
*	Module Information
*/
MODULE_LICENSE("GPL");            
MODULE_AUTHOR("RECG - Nisal");    
MODULE_DESCRIPTION("A Dummy driver for comp 513 labs");  
MODULE_VERSION("0.1");           



/* 
*	The prototype functions for the character driver -- must come before the struct definition
*/
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


/*
*	Functions to access from the kernel
*/
static int     kdev_open(void);
static int     kdev_release(void);
static int 	   kdev_read(int * );
static int 	   kdev_write(int *, int len);

static int 	   reset_kdummy(void);
static int     get_no_det_cals(void);

EXPORT_SYMBOL(kdev_open);
EXPORT_SYMBOL(kdev_release);
EXPORT_SYMBOL(kdev_read);
EXPORT_SYMBOL(kdev_write);
EXPORT_SYMBOL(reset_kdummy);
EXPORT_SYMBOL(get_no_det_cals);



/*
*	Stores the device number -- determined automatically
*/
static int    majorNumber;                  
static int  size_of_message = sizeof(int)*D_ARRAY_SIZE*D_ARRAY_SIZE;              


/*
*	Counts the number of times the device is opened
*/
int    numberOpens = 0;  

/*
*	The device-driver class struct pointer
*/          		
static struct class*  dummycharClass  = NULL; 

/*
*	The device-driver device struct pointer
*/		
static struct device* dummycharDevice = NULL; 		


/*
*	position of the ring buffer
*/
atomic_t current_buf_pos;
/*
*	position of the results buffer
*/
atomic_t current_det_pos;


/*
*	position of the results buffer
*/
atomic_t correct_det_calculations;

/*
*	sysfs - number of reads, number of correct det cals
*/
static int no_of_reads;
static int no_of_det_cals;



/*tolerance */
const int tolerance = 10000; //(.01%)

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
*	sysfs set function
*/
static int param_set_no_of_reads (const char *val, const struct kernel_param *kp)
{
	
	int ret, tmp;
	int i; 
	char *ctemp, *stemp;


	if (val[0] == '0') {
		tmp = 0;
		sprintf(val, "%d", tmp);
		ret = param_set_int(val, kp);
		atomic_set(&current_buf_pos, 0);
		atomic_set(&current_det_pos, 0);
		atomic_set(&correct_det_calculations, 0);
		printk(KERN_INFO "Dummy reset successful %d, %d, %d\n", atomic_read(&current_buf_pos), atomic_read(&current_det_pos), atomic_read(&correct_det_calculations) );
	}
	else {
		printk(KERN_INFO "Error in coverting str to int \n", val[0]);
		return -EINVAL;
	}

	return 0;

}

static int param_get_no_of_reads (char *val, const struct kernel_param *kp)
{	
	no_of_reads = atomic_read(&current_buf_pos);
	printk(KERN_INFO "getting no_of_reads %d \n", no_of_reads);
	return sprintf(val,"%d\n", no_of_reads);

}

static struct kernel_param_ops params_ops_no_of_reads = {
	   .set = param_set_no_of_reads,
	   .get = param_get_no_of_reads,
	   
};
module_param_cb(no_of_reads, &params_ops_no_of_reads, &no_of_reads, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
//module_param(no_of_reads, int ,S_IRUSR|S_IWUSR);


static int param_get_no_of_det_cals (char *val, const struct kernel_param *kp)
{	
	no_of_det_cals = atomic_read(&correct_det_calculations);
	printk(KERN_INFO "getting no_of_det_cals %d\n", no_of_det_cals);
	return sprintf(val,"%d\n", no_of_det_cals);

}

static struct kernel_param_ops params_ops_no_of_det_cals = {
	   //.set = param_set_no_of_reads,
	   .get = param_get_no_of_det_cals,
	   

};
module_param_cb(no_of_det_cals, &params_ops_no_of_det_cals, &no_of_det_cals, S_IRUSR);



static struct file_operations fops =
{
	   .open = dev_open,
	   .read = dev_read,
	   .write = dev_write,
	   .release = dev_release,
};

/*
*	inirializtion function
*/
static int __init dummychar_init(void)
{

	/*
	*	initializing dummy will reset the counter
	*/
	atomic_set(&current_buf_pos, 0);
	atomic_set(&current_det_pos, 0);
	atomic_set(&correct_det_calculations, 0);
	

	printk(KERN_INFO "dummy: Initializing the dummy LKM\n");

	/*
	*	Try to dynamically allocate a major number for the device -- more difficult but worth it
	*/
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
	  printk(KERN_ALERT "dummy failed to register a major number\n");
	  return majorNumber;
	}
	printk(KERN_INFO "dummy: registered correctly with major number %d\n", majorNumber);

	/*
	*	Register the device class
	*/
	dummycharClass = class_create(THIS_MODULE, CLASS_NAME);

	/*
	*	Check for error and clean up if there is
	*/
	if (IS_ERR(dummycharClass)){                			
	  unregister_chrdev(majorNumber, DEVICE_NAME);
	  printk(KERN_ALERT "Failed to register device class\n");
	  return PTR_ERR(dummycharClass);          			
	}
	printk(KERN_INFO "dummy: device class registered correctly\n");

	/*
	*	Register the device driver
	*/
	dummycharDevice = device_create(dummycharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	/*
	*	Clean up if there is an error
	*/
	if (IS_ERR(dummycharDevice)){               			
	  class_destroy(dummycharClass);           			
	  unregister_chrdev(majorNumber, DEVICE_NAME);
	  printk(KERN_ALERT "Failed to create the device\n");
	  return PTR_ERR(dummycharDevice);
	}
	printk(KERN_INFO "dummy: device class created correctly\n"); 	

		 
	return 0;
}

/*
*	cleanup function
*/
static void __exit dummychar_exit(void)
{
	device_destroy(dummycharClass, MKDEV(majorNumber, 0));     	
	class_unregister(dummycharClass);                          	
	class_destroy(dummycharClass);                             	
	unregister_chrdev(majorNumber, DEVICE_NAME);             	

	atomic_set(&current_buf_pos, 0);
	printk(KERN_INFO "dummy: current_buf_pos %d \n", atomic_read(&current_buf_pos));

	printk(KERN_INFO "dummy: Goodbye from the LKM!\n");
}


static int dev_open(struct inode *inodep, struct file *filep)
{
	numberOpens++;
	printk(KERN_INFO "dummy: Device has been opened %d time(s)\n", numberOpens);
	return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int error_count = 0;
	int tmp_buf_pos;
	size_of_message = sizeof(int)*D_ARRAY_SIZE*D_ARRAY_SIZE; 
	printk(KERN_INFO "dummy: inside dev read\n");

	tmp_buf_pos = atomic_read(&current_buf_pos)% NO_OF_ARRAYS ;
	atomic_add(1, &current_buf_pos);
	//no_of_reads++;

	//test code to printout the array values
	/*
	printk(KERN_INFO "current_buf_pos %d\n", tmp_buf_pos);
	for (i=0; i< D_ARRAY_SIZE*D_ARRAY_SIZE; i++) {
	printk(KERN_INFO "%d\t", d_buffer[tmp_buf_pos][i]);
	} 
	printk(KERN_INFO "\n");
	*/

	error_count = copy_to_user(buffer, d_buffer[tmp_buf_pos], size_of_message); 
	/*if true then have success*/
	if (error_count==0){            
		//printk(KERN_INFO "dummy: Sent %d characters to the user, current buf pos %d\n", size_of_message, current_buf_pos);
		return (size_of_message=0);  
	}
	else {
		printk(KERN_INFO "dummy: Failed to send %d characters to the user\n", error_count);
		/* Failed -- return a bad address message (i.e. -14)*/
		return -EFAULT;              
	}
}


static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	long long det_current_mat, tmp_det_result, tol_range;

	int error_count = 0;
	size_of_message = sizeof(long long);
	//printk(KERN_INFO "dummy: inside dev write\n");

	error_count = copy_from_user(&det_current_mat, buffer, len); 
	if (error_count==0){         

		
		tmp_det_result = det_results [atomic_read(&current_det_pos) % NO_OF_ARRAYS];
		tol_range = det_current_mat/tolerance;
		printk(KERN_INFO "dummy: read from the user  %lld %lld\n", det_current_mat, tmp_det_result);

		/**




		//printk(KERN_INFO "dummy: read from the user  %lld %lld\n", det_current_mat, tmp_det_result);
		/* Correct determinant result, return 0
		accept only certain value  */ 
		if (det_current_mat == tmp_det_result) {
			atomic_add(1, &current_det_pos);
			atomic_add(1, &correct_det_calculations);
			return 0;
		}

		/*accept range of values*/
		if (det_current_mat >= 0) {
			if (det_current_mat > tmp_det_result - tol_range && det_current_mat < tmp_det_result + tol_range){
				atomic_add(1, &current_det_pos);
				atomic_add(1, &correct_det_calculations);
				return 0;
			}
			else return -1;
		} 

		if (det_current_mat < 0) {
			if (det_current_mat > tmp_det_result + tol_range && det_current_mat < tmp_det_result - tol_range){
				atomic_add(1, &current_det_pos);
				atomic_add(1, &correct_det_calculations);
				return 0;
			}
			else return -1;
		} 




		/*
		* Incorrect determinant result, return -1
		*/
		return -1;


		return (size_of_message);  
	}
	else {
		printk(KERN_INFO "dummy: Failed to read %d characters from the user\n", error_count);
		/* Failed -- return a bad address message (i.e. -14)*/
		return -EFAULT;              
	}


}




static int dev_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "dummy: Device successfully closed\n");
	numberOpens--;
	return 0;
}


module_init(dummychar_init);
module_exit(dummychar_exit);


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int kdev_open(void)
{
	numberOpens++;
	printk(KERN_INFO "dummy: Device has been opened %d time(s)\n", numberOpens);
	return 0;
}

static int kdev_release(void)
{
	printk(KERN_INFO "dummy: Device successfully closed\n");
	numberOpens--;
	return 0;
}

static int kdev_read(int *buffer)
{
	int error_count = 0;
	int tmp_buf_pos;
	size_of_message = sizeof(int)*D_ARRAY_SIZE*D_ARRAY_SIZE; 
	printk(KERN_INFO "dummy: inside dev read\n");

	tmp_buf_pos = atomic_read(&current_buf_pos)% NO_OF_ARRAYS ;
	atomic_add(1, &current_buf_pos);


	memcpy(buffer, d_buffer[tmp_buf_pos], size_of_message); 

	return 0;

}



static int kdev_write(int *buffer, int len)
{
	long long det_current_mat, tmp_det_result, tol_range;

	int error_count = 0;
	size_of_message = sizeof(long long);
	//printk(KERN_INFO "dummy: inside dev write\n");

	error_count = 0;
	memcpy (&det_current_mat, buffer, len); 
	//error_count = copy_from_user(&det_current_mat, buffer, len); 
	if (error_count==0){         

		
		tmp_det_result = det_results [atomic_read(&current_det_pos) % NO_OF_ARRAYS];
		tol_range = det_current_mat/tolerance;
		printk(KERN_INFO "dummy: read from the user  %lld %lld\n", det_current_mat, tmp_det_result);

		/**




		//printk(KERN_INFO "dummy: read from the user  %lld %lld\n", det_current_mat, tmp_det_result);
		/* Correct determinant result, return 0
		accept only certain value  */ 
		if (det_current_mat == tmp_det_result) {
			atomic_add(1, &current_det_pos);
			atomic_add(1, &correct_det_calculations);
			return 0;
		}

		/*accept range of values*/
		if (det_current_mat >= 0) {
			if (det_current_mat > tmp_det_result - tol_range && det_current_mat < tmp_det_result + tol_range){
				atomic_add(1, &current_det_pos);
				atomic_add(1, &correct_det_calculations);
				return 0;
			}
			else return -1;
		} 

		if (det_current_mat < 0) {
			if (det_current_mat > tmp_det_result + tol_range && det_current_mat < tmp_det_result - tol_range){
				atomic_add(1, &current_det_pos);
				atomic_add(1, &correct_det_calculations);
				return 0;
			}
			else return -1;
		} 




		/*
		* Incorrect determinant result, return -1
		*/
		return -1;


		return (size_of_message);  
	}
	else {
		printk(KERN_INFO "dummy: Failed to read %d characters from the user\n", error_count);
		/* Failed -- return a bad address message (i.e. -14)*/
		return -EFAULT;              
	}


}

static int reset_kdummy(void) {
	atomic_set(&current_buf_pos, 0);
	atomic_set(&current_det_pos, 0);
	atomic_set(&correct_det_calculations, 0);

	return 0;
}

static int get_no_det_cals(void)
{	
	int tmp = atomic_read(&correct_det_calculations);
	return tmp;

}


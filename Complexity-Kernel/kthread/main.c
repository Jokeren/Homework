#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/sched.h>

struct task_struct *reader_task;
struct task_struct *writer_task;
struct task_struct *compute_task;
int reader_id, writer_id, compute_id;


static int reader_function(void *data) {
  int count;
  count = 10;
  int i;
  i = 0;
  while (i < count) {
    printk(KERN_INFO"In Kernel Reader Thread %d\n", i);
    ++i;
  }
  return 0;
}


static int writer_function(void *data) {
  int count;
  count = 10;
  int i;
  i = 0;
  while (i < count) {
    printk(KERN_INFO"In Kernel Writer Thread %d\n", i);
    ++i;
  }
  return 0;
}


static int compute_function(void *data) {
  int count;
  count = 10;
  int i;
  i = 0;
  while (i < count) {
    printk(KERN_INFO"In Kernel Compute Thread %d\n", i);
    ++i;
  }
  return 0;
}


static int kernel_init(void) {
  printk(KERN_INFO"--------------------------------------------");
  reader_id = 1;
  writer_id = 2;
  compute_id = 3;
  reader_task = kthread_run(&reader_function, (void *)&reader_id, "reader_task");
  writer_task = kthread_run(&writer_function, (void *)&writer_id, "writer_task");
  compute_task = kthread_run(&compute_function, (void *)&compute_id, "compute_task");
  return 0;
}

static void kernel_exit(void)
{
}

module_init(kernel_init);
module_exit(kernel_exit);

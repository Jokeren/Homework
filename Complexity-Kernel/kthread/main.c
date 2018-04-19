#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>
#include <linux/timer.h>


struct task_struct *reader_task;
struct task_struct *writer_task;
struct task_struct *compute_task;
int reader_id, writer_id, compute_id;
DEFINE_SPINLOCK(compute_read_lock);
DEFINE_SPINLOCK(compute_write_lock);
DEFINE_SPINLOCK(writer_read_lock);
DEFINE_SPINLOCK(writer_write_lock);

static int reader_function(void *data) {
  int count;
  count = 10;
  int i;
  i = 0;
  while (i < count) {
    spin_lock(&compute_write_lock);
    printk(KERN_INFO"In Kernel Reader Thread %d\n", i);
    spin_unlock(&compute_read_lock);
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
    spin_lock(&writer_read_lock);
    printk(KERN_INFO"In Kernel Writer Thread %d\n", i);
    spin_unlock(&writer_write_lock);
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
    spin_lock(&compute_read_lock);
    printk(KERN_INFO"In Kernel Compute Thread %d\n", i);
    spin_unlock(&compute_write_lock);
    spin_lock(&writer_write_lock);
    spin_unlock(&writer_read_lock);
    ++i;
  }
  return 0;
}


static int kernel_init(void) {
  printk(KERN_INFO"--------------------------------------------");
  reader_id = 1;
  writer_id = 2;
  compute_id = 3;
  spin_lock(&compute_read_lock);
  spin_lock(&writer_read_lock);
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

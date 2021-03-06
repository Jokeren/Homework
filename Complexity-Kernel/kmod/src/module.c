#include <linux/init.h> 
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/time.h>
#include <linux/kernel.h> 
#include <linux/kthread.h>  // for threads
#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/mm.h> 
#include <linux/delay.h>
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>
#include <linux/atomic.h>
#include <linux/mutex.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Keren Zhou <kz21@rice.edu>");
MODULE_DESCRIPTION("A simple kernel module for determinant calculation");
MODULE_VERSION("0.1.0");

#include <asm/uaccess.h> 

#define MODULE_DEBUG 1
#ifdef MODULE_DEBUG
#define PRINT(...) printk(__VA_ARGS__)
#else
#define PRINT(...)
#endif

#define NUM_COMP_THREADS 30

// entry functions
extern void rust_init(void);
extern void rust_cleanup(void);
extern void rust_reader(void);
extern void rust_writer(void);
extern void rust_measure(void);
extern void rust_compute(int thread_id);

struct task_struct *reader_task;
struct task_struct *writer_task;
struct task_struct *measure_task;
struct task_struct *compute_task[NUM_COMP_THREADS];
int reader_id;
int writer_id;
int measure_id;
int compute_id[NUM_COMP_THREADS];
atomic_t reader_terminate_flag;
atomic_t writer_terminate_flag;
atomic_t measure_terminate_flag;
atomic_t compute_terminate_flag[NUM_COMP_THREADS];
struct mutex writer_read_lock[NUM_COMP_THREADS];
struct mutex writer_write_lock[NUM_COMP_THREADS];
struct mutex compute_read_lock[NUM_COMP_THREADS];
struct mutex compute_write_lock[NUM_COMP_THREADS];

static int m_init_module(void) {
  rust_init();
}


static void m_cleanup_module(void) {
  rust_cleanup();
}


extern void kschedule_timeout(int time) {
  schedule_timeout_interruptible(time);
}

extern long long kget_time(void) {
  struct timespec time;
  getnstimeofday(&time);
  return time.tv_sec;
}

// borrow from http://ytliu.info/notes/linux/file_ops_in_kernel.html
extern void kwrite_file(int num) {
  PRINT("Module->Open file...\n");
  static const char *filename = "/tmp/kz21.output";
	char data[16];
  sprintf(data, "%d\n", num);
  struct file *filp = filp_open(filename, O_CREAT|O_WRONLY|O_APPEND, 0777);
  if(IS_ERR(filp)) {
    PRINT("Module->Open error...\n");
    return;
  }
	PRINT("Module->Start write %d to file...\n", num);
  mm_segment_t fs = get_fs();
  set_fs(KERNEL_DS);
	vfs_write(filp, data, strlen(data), &filp->f_pos);
  filp_close(filp,NULL);
  set_fs(fs);
	PRINT("Module->Write complete...\n");
}


extern void ksleep(int time) {
  msleep_interruptible(time);
}


extern bool kcompute_write_trylock(int thread_id) {
  return mutex_trylock(&compute_write_lock[thread_id]);
}


extern void kcompute_write_lock(int thread_id) {
  mutex_lock(&compute_write_lock[thread_id]);
}


extern void kcompute_write_unlock(int thread_id) {
  mutex_unlock(&compute_write_lock[thread_id]);
}


extern bool kcompute_read_trylock(int thread_id) {
  return mutex_trylock(&compute_read_lock[thread_id]);
}


extern void kcompute_read_lock(int thread_id) {
  mutex_lock(&compute_read_lock[thread_id]);
}


extern void kcompute_read_unlock(int thread_id) {
  mutex_unlock(&compute_read_lock[thread_id]);
}


extern bool kwriter_write_trylock(int thread_id) {
  return mutex_trylock(&writer_write_lock[thread_id]);
}


extern void kwriter_write_lock(int thread_id) {
  mutex_lock(&writer_write_lock[thread_id]);
}


extern void kwriter_write_unlock(int thread_id) {
  mutex_unlock(&writer_write_lock[thread_id]);
}


extern bool kwriter_read_trylock(int thread_id) {
  return mutex_trylock(&writer_read_lock[thread_id]);
}


extern void kwriter_read_lock(int thread_id) {
  mutex_lock(&writer_read_lock[thread_id]);
}


extern void kwriter_read_unlock(int thread_id) {
  mutex_unlock(&writer_read_lock[thread_id]);
}


extern bool kcompute_get_terminate(int thread_id) {
  return atomic_read(&compute_terminate_flag[thread_id]) == 1;
}


extern void kcompute_set_terminate(int thread_id) {
  atomic_set(&compute_terminate_flag[thread_id], 1);
}


extern bool kreader_get_terminate(void) {
  return atomic_read(&reader_terminate_flag) == 1;
}


extern void kreader_set_terminate(void) {
  atomic_set(&reader_terminate_flag, 1);
}


extern bool kmeasure_get_terminate(void) {
  return atomic_read(&measure_terminate_flag) == 1;
}


extern void kmeasure_set_terminate(void) {
  atomic_set(&measure_terminate_flag, 1);
}


extern bool kwriter_get_terminate(void) {
  return atomic_read(&writer_terminate_flag) == 1;
}


extern void kwriter_set_terminate(void) {
  atomic_set(&writer_terminate_flag, 1);
}


extern int reader_function(void *data) {
  rust_reader();
  return 0;
}


extern int writer_function(void *data) {
  rust_writer();
  return 0;
}


extern int compute_function(void *data) {
  int thread_id = *((int *)data);
  rust_compute(thread_id);
  return 0;
}


extern int measure_function(void *data) {
  rust_measure();
  return 0;
}


extern void kinit_thread(void) {
  printk(KERN_INFO"--------------------------------------------");

  reader_id = NUM_COMP_THREADS;
  writer_id = NUM_COMP_THREADS + 1;
  measure_id = NUM_COMP_THREADS + 2;

  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    mutex_init(&compute_read_lock[i]);
    mutex_init(&compute_write_lock[i]);
    mutex_init(&writer_read_lock[i]);
    mutex_init(&writer_write_lock[i]);
    mutex_lock(&compute_read_lock[i]);
    mutex_lock(&writer_read_lock[i]);
  }

  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    atomic_set(&compute_terminate_flag[i], 0);
  }
  atomic_set(&reader_terminate_flag, 0);
  atomic_set(&writer_terminate_flag, 0);
  atomic_set(&measure_terminate_flag, 0);

  struct sched_param param = { .sched_priority = MAX_USER_RT_PRIO - 1 };

  //reader_task = kthread_run(&reader_function, (void *)&reader_id, "reader_task");
  reader_task = kthread_create(&reader_function, (void *)&reader_id, "reader_task");
  sched_setscheduler(reader_task, SCHED_RR, &param);
  wake_up_process(reader_task);

  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    compute_id[i] = i;
    //compute_task[i] = kthread_run(&compute_function, (void *)&compute_id[i], "compute_task");
    compute_task[i] = kthread_create(&compute_function, (void *)&compute_id[i], "compute_task");
    sched_setscheduler(compute_task[i], SCHED_RR, &param);
    wake_up_process(compute_task[i]);
  }

  //writer_task = kthread_run(&writer_function, (void *)&writer_id, "writer_task");
  writer_task = kthread_create(&writer_function, (void *)&writer_id, "writer_task");
  sched_setscheduler(writer_task, SCHED_RR, &param);
  wake_up_process(writer_task);

  //measure_task = kthread_run(&measure_function, (void *)&measure_id, "measure_task");
  measure_task = kthread_create(&measure_function, (void *)&measure_id, "measure_task");
  sched_setscheduler(measure_task, SCHED_RR, &param);
  wake_up_process(measure_task);
}

module_init(m_init_module);
module_exit(m_cleanup_module);

//! Common kernel functions

extern "C" {
    // kdev funcs
    pub fn kdev_open() -> i32;
    pub fn kdev_release() -> i32;
    pub fn kdev_read(buf: *mut i32) -> i32;
    pub fn kdev_write(buf: *mut i64, size: i32) -> i32;
    pub fn reset_kdummy() -> i32;
    pub fn get_no_det_cals() -> i32;
    // utils
    pub fn printk(fmt: *const u8, ...) -> i32;
    pub fn kget_time() -> i64;
		pub fn kwrite_file(data: i32);
    pub fn kinit_thread();
    pub fn ksleep(time: i32);
    pub fn kschedule_timeout(time: i32);
    // locks
    pub fn kcompute_write_trylock(thread_id: i32) -> bool;
    pub fn kcompute_write_lock(thread_id: i32);
    pub fn kcompute_write_unlock(thread_id: i32);
    pub fn kcompute_read_trylock(thread_id: i32) -> bool;
    pub fn kcompute_read_lock(thread_id: i32);
    pub fn kcompute_read_unlock(thread_id: i32);
    pub fn kwriter_write_trylock(thread_id: i32) -> bool;
    pub fn kwriter_write_lock(thread_id: i32);
    pub fn kwriter_write_unlock(thread_id: i32);
    pub fn kwriter_read_trylock(thread_id: i32) -> bool;
    pub fn kwriter_read_lock(thread_id: i32);
    pub fn kwriter_read_unlock(thread_id: i32);
    // flags
    pub fn kcompute_get_terminate(thread_id: i32) -> bool;
    pub fn kreader_get_terminate() -> bool;
    pub fn kwriter_get_terminate() -> bool;
    pub fn kmeasure_get_terminate() -> bool;
    pub fn kcompute_set_terminate(thread_id: i32);
    pub fn kreader_set_terminate();
    pub fn kwriter_set_terminate();
    pub fn kmeasure_set_terminate();
}

// Common printing macro
#[macro_export] 
macro_rules! print {
    ($fmt:expr) => ({
        unsafe { ::kdev::printk(concat!($fmt, "\0").as_ptr()); }
    });
    ($fmt:expr, $($arg:tt)+) => ({
        unsafe { ::kdev::printk(concat!($fmt, "\0").as_ptr(), $($arg)*); }
    });
}

// Common printing macro including newline
#[macro_export] 
macro_rules! println {
  ($fmt:expr)              => (print!(concat!($fmt, "\n")));
  ($fmt:expr, $($arg:tt)+) => (print!(concat!($fmt, "\n"), $($arg)*));
}


pub fn get_time() -> i64 {
  let ret: i64 = unsafe {
    kget_time()
  };
  return ret;
}


pub fn write_file(num: i32) {
	unsafe {
		kwrite_file(num);
	}
}


pub fn init_thread() {
  unsafe {
    kinit_thread();
  }
}


pub fn sleep(time: i32) {
  unsafe {
    ksleep(time);
  }
}


pub fn compute_write_trylock(thread_id: i32) -> bool {
  let ret = unsafe {
    kcompute_write_trylock(thread_id)
  };
  return ret;
}


pub fn compute_write_lock(thread_id: i32) {
  unsafe {
    kcompute_write_lock(thread_id);
  }
}


pub fn compute_write_unlock(thread_id: i32) {
  unsafe {
    kcompute_write_unlock(thread_id);
  }
}


pub fn compute_read_trylock(thread_id: i32) -> bool {
  let ret = unsafe {
    kcompute_read_trylock(thread_id)
  };
  return ret;
}


pub fn compute_read_lock(thread_id: i32) {
  unsafe {
    kcompute_read_lock(thread_id);
  }
}


pub fn compute_read_unlock(thread_id: i32) {
  unsafe {
    kcompute_read_unlock(thread_id);
  }
}


pub fn writer_write_trylock(thread_id: i32) -> bool{
  let ret = unsafe {
    kwriter_write_trylock(thread_id)
  };
  return ret;
}


pub fn writer_write_lock(thread_id: i32) {
  unsafe {
    kwriter_write_lock(thread_id);
  }
}


pub fn writer_write_unlock(thread_id: i32) {
  unsafe {
    kwriter_write_unlock(thread_id);
  }
}


pub fn writer_read_trylock(thread_id: i32) -> bool{
  let ret = unsafe {
    kwriter_read_trylock(thread_id)
  };
  return ret;
}


pub fn writer_read_lock(thread_id: i32) {
  unsafe {
    kwriter_read_lock(thread_id);
  }
}


pub fn writer_read_unlock(thread_id: i32) {
  unsafe {
    kwriter_read_unlock(thread_id);
  }
}


pub fn compute_get_terminate(thread_id: i32) -> bool {
  let ret = unsafe {
    kcompute_get_terminate(thread_id)
  };
  return ret;
}


pub fn compute_set_terminate(thread_id: i32) {
  unsafe {
    kcompute_set_terminate(thread_id);
  }
}


pub fn measure_get_terminate() -> bool {
  let ret = unsafe {
    kmeasure_get_terminate()
  };
  return ret;
}


pub fn measure_set_terminate() {
  unsafe {
    kmeasure_set_terminate();
  }
}


pub fn writer_get_terminate() -> bool {
  let ret = unsafe {
    kwriter_get_terminate()
  };
  return ret;
}


pub fn writer_set_terminate() {
  unsafe {
    kwriter_set_terminate();
  }
}


pub fn reader_get_terminate() -> bool {
  let ret = unsafe {
    kreader_get_terminate()
  };
  return ret;
}


pub fn reader_set_terminate() {
  unsafe {
    kreader_set_terminate();
  }
}


pub fn schedule(time: i32) {
  unsafe {
    kschedule_timeout(time);
  }
}

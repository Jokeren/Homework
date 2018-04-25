//! A simple kernel module in Rust
//#![feature(lang_items, core_str_ext)]
#![feature(lang_items)]
//#![deny(missing_docs, warnings)]
//#![deny(missing_docs)]
#![allow(unused_features)]
#![no_std]

#[macro_use]
mod kdev;
mod determinant;
pub mod lang_items;

use determinant::MATRIX_LEN;
use determinant::Fraction;
use determinant::determinant_fn; 

const N_WORKERS: usize = 30;
const BULK_SIZE: usize = 1;
const LIFE: usize = 180;
const TOTAL: usize = 250000;

static mut BUF1: [[[i32; MATRIX_LEN * MATRIX_LEN]; BULK_SIZE]; N_WORKERS] = [[[0; MATRIX_LEN * MATRIX_LEN]; BULK_SIZE]; N_WORKERS];
static mut BUF2: [[[i32; MATRIX_LEN * MATRIX_LEN]; BULK_SIZE]; N_WORKERS] = [[[0; MATRIX_LEN * MATRIX_LEN]; BULK_SIZE]; N_WORKERS];
static mut FRACTIONS : [[[Fraction; MATRIX_LEN * MATRIX_LEN]; BULK_SIZE]; N_WORKERS] = [[[Fraction { numerator: 0, denominator: 0}; MATRIX_LEN * MATRIX_LEN]; BULK_SIZE]; N_WORKERS];
static mut RESULTS: [[i64; BULK_SIZE]; N_WORKERS] = [[0; BULK_SIZE]; N_WORKERS];
static mut WRITE_DETS : [i64; BULK_SIZE] = [0; BULK_SIZE];
static mut COMPUTE_DETS : [[i64; BULK_SIZE]; N_WORKERS] = [[0; BULK_SIZE]; N_WORKERS];
static mut RETS : [i32; LIFE * 2] = [0; LIFE * 2];


#[no_mangle]
pub fn rust_reader() {
  println!("Reader thread start");
  let mut count = 0;
  while kdev::measure_get_terminate() == false {
    //kdev::schedule(0);
    unsafe {
      if count == 0 {
        for t in 0..N_WORKERS {
          for b in 0..BULK_SIZE {
            kdev::kdev_read(BUF1.get_unchecked_mut(t).get_unchecked_mut(b).get_unchecked_mut(0) as *mut i32);
          }
        }
      } else {
        for t in 0..N_WORKERS {
          for b in 0..BULK_SIZE {
            kdev::kdev_read(BUF2.get_unchecked_mut(t).get_unchecked_mut(b).get_unchecked_mut(0) as *mut i32);
          }
        }
      }
    }

    for t in 0..N_WORKERS {
      while kdev::compute_write_trylock(t as i32) == false {
        if kdev::measure_get_terminate() == true {
          kdev::reader_set_terminate();
          return;
        }
        kdev::schedule(0);
      }
      kdev::compute_read_unlock(t as i32);
    }

    // do not use %
    count = count + 1;
    if count == 2 {
      count = 0;
    }
  }
  kdev::reader_set_terminate();
}

#[no_mangle]
pub fn rust_writer() {
  println!("Writer thread start");
  let mut order = 0;
  while kdev::measure_get_terminate() == false {
    //kdev::schedule(0);
    while kdev::writer_read_trylock(order) == false {
      if kdev::measure_get_terminate() == true {
        kdev::writer_set_terminate();
        return;
      }
      kdev::schedule(0);
    }

    for b in 0..BULK_SIZE {
      unsafe {
        *WRITE_DETS.get_unchecked_mut(b) = RESULTS.get_unchecked(order as usize).get_unchecked(b).clone();
      };
    }

    kdev::writer_write_unlock(order);

    for b in 0..BULK_SIZE {
      unsafe {
        let mut det = WRITE_DETS.get_unchecked(b).clone();
        kdev::kdev_write(&mut det as *mut i64, 8);
      }
    }

    order = order + 1;
    if order == N_WORKERS as i32 {
      order = 0;
    }
  }
  kdev::writer_set_terminate();
}

#[no_mangle]
pub fn rust_compute(thread_id: i32) {
  println!("Compute thread start %d", thread_id);
  let mut count = 0;
  while kdev::measure_get_terminate() == false {
    while kdev::compute_read_trylock(thread_id) == false {
      if kdev::measure_get_terminate() == true {
        kdev::compute_set_terminate(thread_id);
        println!("Compute thread terminate %d", thread_id);
        return;
      }
      kdev::schedule(0);
    }

    unsafe {
      if count == 0 {
        for b in 0..BULK_SIZE {
          let mut i = 0;
          while i < MATRIX_LEN * MATRIX_LEN {
            let numerator = BUF1.get_unchecked(thread_id as usize).get_unchecked(b).get_unchecked(i).clone() as i128;
            *FRACTIONS.get_unchecked_mut(thread_id as usize).get_unchecked_mut(b).get_unchecked_mut(i) = Fraction { numerator: numerator, denominator: 1};
            i = i + 1;
          }
        }
      } else {
        for b in 0..BULK_SIZE {
          let mut i = 0;
          while i < MATRIX_LEN * MATRIX_LEN {
            let numerator = BUF2.get_unchecked(thread_id as usize).get_unchecked(b).get_unchecked(i).clone() as i128;
            *FRACTIONS.get_unchecked_mut(thread_id as usize).get_unchecked_mut(b).get_unchecked_mut(i) = Fraction { numerator: numerator, denominator: 1};
            i = i + 1;
          }
        }
      }
    }

    kdev::compute_write_unlock(thread_id);

    for b in 0..BULK_SIZE {
      let mut det: i64 = unsafe {
        determinant_fn(&mut FRACTIONS.get_unchecked_mut(thread_id as usize).get_unchecked_mut(b)) 
      };
      unsafe {
        *COMPUTE_DETS.get_unchecked_mut(thread_id as usize).get_unchecked_mut(b) = det;
      }
      //kdev::schedule(0);
    }

    while kdev::writer_write_trylock(thread_id) == false {
      if kdev::measure_get_terminate() == true {
        kdev::compute_set_terminate(thread_id);
        println!("Compute thread terminate %d", thread_id);
        return;
      }
      kdev::schedule(0);
    }
    for b in 0..BULK_SIZE {
      unsafe {
        *RESULTS.get_unchecked_mut(thread_id as usize).get_unchecked_mut(b) = COMPUTE_DETS.get_unchecked(thread_id as usize).get_unchecked(b).clone();
      }
    }
    kdev::writer_read_unlock(thread_id);

    count = count + 1;
    if count == 2 {
      count = 0;
    }
  }
  kdev::compute_set_terminate(thread_id);
}

#[no_mangle]
pub fn rust_measure() {
  println!("Measure thread start");
  let init_time: i64 = kdev::get_time();
  let prev_time: i64 = kdev::get_time();
  let mut time: i64 = kdev::get_time();
  let mut prev_num = 0;
  let mut count = 0;
  let mut sum = 0;
  while time - prev_time < LIFE as i64 {
    kdev::sleep(100);
    let ret : i32 = unsafe {
      kdev::get_no_det_cals() 
    };
    let curr_num = ret - prev_num;
    sum = sum + curr_num;
    prev_num = ret;
    //println!("Lib->Number correction determinants %d", curr_num);
    time = kdev::get_time();
    unsafe {
      *RETS.get_unchecked_mut(count) = curr_num;
    }
    count = count + 1;
    if sum >= TOTAL as i32 {
      break;
    }
  }
  let final_time: i64 = kdev::get_time();
  println!("Lib->Calculation Time %lld s\n", final_time - init_time);
  kdev::measure_set_terminate();
  println!("Lib->Wait for reader terminate");
  while kdev::reader_get_terminate() == false {}
  println!("Lib->Wait for writer terminate");
  while kdev::writer_get_terminate() == false {}
  println!("Lib->Wait for compute terminate");
  for i in 0..N_WORKERS {
    while kdev::compute_get_terminate(i as i32) == false {}
    println!("Lib->Compute thread %d released", i);
  }
  println!("Lib->Release locks");
  for i in 0..N_WORKERS {
    kdev::compute_read_trylock(i as i32);
    kdev::compute_read_unlock(i as i32);
    kdev::compute_write_trylock(i as i32);
    kdev::compute_write_unlock(i as i32);
    kdev::writer_read_trylock(i as i32);
    kdev::writer_read_unlock(i as i32);
    kdev::writer_write_trylock(i as i32);
    kdev::writer_write_unlock(i as i32);
  }
  for i in 0..count {
    let ret = unsafe {
      RETS.get_unchecked(i).clone()
    };
    kdev::write_file(ret);
  }
}


/// Entry point for the kernel module
#[no_mangle]
pub fn rust_init() -> i32 {
  println!("Lib->Module initialize...");
  unsafe {
    kdev::kdev_open();
    kdev::reset_kdummy();
  }
  kdev::init_thread();
  return 0;
}

/// Exit point for the kernel module
#[no_mangle]
pub fn rust_cleanup() {
  unsafe {
    kdev::kdev_release();
  }
  println!("Lib->Module exit...");
}

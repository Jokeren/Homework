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

const N_WORKERS: usize = 2;

static mut BUF1: [[i32; MATRIX_LEN * MATRIX_LEN]; N_WORKERS] = [[0; MATRIX_LEN * MATRIX_LEN]; N_WORKERS];
static mut BUF2: [[i32; MATRIX_LEN * MATRIX_LEN]; N_WORKERS] = [[0; MATRIX_LEN * MATRIX_LEN]; N_WORKERS];
static mut FRACTIONS : [[Fraction; MATRIX_LEN * MATRIX_LEN]; N_WORKERS] = [[Fraction { numerator: 0, denominator: 0}; MATRIX_LEN * MATRIX_LEN]; N_WORKERS];
static mut RESULTS: [i64; N_WORKERS] = [0; N_WORKERS];

#[no_mangle]
pub fn rust_reader() {
  println!("Reader thread start");
  let mut count = 0;
  while kdev::measure_get_terminate() == false {
    unsafe {
      for t in 0..N_WORKERS {
        if count == 0 {
          kdev::kdev_read(&mut BUF1[t][0] as *mut i32);
        } else {
          kdev::kdev_read(&mut BUF2[t][0] as *mut i32);
        }
      }
    }

    for t in 0..N_WORKERS {
      while kdev::compute_write_trylock(t as i32) == false {
        if kdev::measure_get_terminate() == true {
          kdev::reader_set_terminate();
          return;
        }
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
    while kdev::writer_read_trylock(order) == false {
      if kdev::measure_get_terminate() == true {
        kdev::writer_set_terminate();
        return;
      }
    }

    let mut det: i64 = unsafe {
      RESULTS.get_unchecked(order as usize).clone()
    };
    kdev::writer_write_unlock(order);
    unsafe {
      kdev::kdev_write(&mut det as *mut i64, 8);
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
        println!("terminate %d", thread_id);
        return;
      }
    }
    unsafe {
      let mut i = 0;
      if count == 0 {
        while i < MATRIX_LEN * MATRIX_LEN {
          let numerator = BUF1.get_unchecked(thread_id as usize).get_unchecked(i).clone() as i128;
          *FRACTIONS.get_unchecked_mut(thread_id as usize).get_unchecked_mut(i) = Fraction { numerator: numerator, denominator: 1};
          i = i + 1;
        }
      } else {
        while i < MATRIX_LEN * MATRIX_LEN {
          let numerator = BUF2.get_unchecked(thread_id as usize).get_unchecked(i).clone() as i128;
          *FRACTIONS.get_unchecked_mut(thread_id as usize).get_unchecked_mut(i) = Fraction { numerator: numerator, denominator: 1};
          i = i + 1;
        }
      }
    }
    kdev::compute_write_unlock(thread_id);

    let mut det: i64 = unsafe {
      determinant_fn(&mut FRACTIONS.get_unchecked_mut(thread_id as usize)) 
    };

    while kdev::writer_write_trylock(thread_id) == false {
      if kdev::measure_get_terminate() == true {
        kdev::compute_set_terminate(thread_id);
        println!("terminate %d", thread_id);
        return;
      }
    }
    unsafe {
      *RESULTS.get_unchecked_mut(thread_id as usize) = det;
    }
    kdev::writer_read_unlock(thread_id);

    count = count + 1;
    if count == 2 {
      count = 0;
    }
    kdev::schedule(1000);
    let flag = kdev::measure_get_terminate();
    println!("Compute thread %d terminate_flag %d", thread_id, flag as i32);
  }
  kdev::compute_set_terminate(thread_id);
}

#[no_mangle]
pub fn rust_measure() {
  println!("Measure thread start");
  let prev_time: i64 = kdev::get_time();
  let mut time: i64 = kdev::get_time();
  let mut prev_num = 0;
  let total = 1;
  while time - prev_time < total {
    kdev::sleep(1000);
    let ret : i32 = unsafe {
      kdev::get_no_det_cals() 
    };
    kdev::write_file(ret);
    let curr_num = ret - prev_num;
    prev_num = ret;
    println!("Lib->Number correction determinants %d", curr_num);
    time = kdev::get_time();
  }
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
}


/// Entry point for the kernel module
#[no_mangle]
pub fn init_module() -> i32 {
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
pub fn cleanup_module() {
  unsafe {
    kdev::kdev_release();
  }
  println!("Lib->Module exit...");
}

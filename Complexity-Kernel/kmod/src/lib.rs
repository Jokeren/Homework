//! A simple kernel module in Rust
#![feature(lang_items, core_str_ext)]
#![deny(missing_docs, warnings)]
#![allow(unused_features)]
#![no_std]

#[macro_use]
mod print;
mod kdev;
pub mod lang_items;

const MATRIX_LEN: usize = 32;
static mut BUF: [i32; MATRIX_LEN * MATRIX_LEN] = [0; MATRIX_LEN * MATRIX_LEN];

#[no_mangle]
/// Entry point for the kernel module
pub fn init_module() -> i32 {
    println!("lib: Module initialized.");
    unsafe {
      kdev::kdev_open();
    }
    unsafe {
      kdev::reset_kdummy();
    }
    let ret = unsafe {
      kdev::kdev_read(&mut BUF[0] as *mut i32)
    };
    println!("lib: Read result %d", ret);
    println!("lib: Element[0] %d", BUF[0]);
    println!("lib: Element[1] %d", BUF[1]);
    let mut ans1: i64 = -77187305771648;
    unsafe {
      kdev::kdev_write(&mut ans1 as *mut i64, 8);
    }
    let ret = unsafe {
      kdev::get_no_det_cals() 
    };
    println!("lib: Number correction determinants %d", ret);
    unsafe {
   	  kdev::kdev_release();
    }
    return 0;
}

#[no_mangle]
/// Exit point for the kernel module
pub fn cleanup_module() {
    println!("Module exit.");
}

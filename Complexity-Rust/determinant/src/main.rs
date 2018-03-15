extern crate threadpool;

use std::env;
use std::fs::OpenOptions;
use std::fs::File;
use std::io::prelude::*;
use std::io::Cursor;
use std::mem;
use std::thread;
use std::time::Duration;

use threadpool::ThreadPool;

const MATRIX_LEN: usize = 32;
const N_WORKERS: usize = 8;
const LIFE: i32 = 1;
const TEN_SEC: u64 = 1;

fn measurement(tid: &thread::ThreadId) {
    println!("[{:?}]->Start measurement...", tid);
		let result = String::from("result.output");
    let mut result = OpenOptions::new()
                                 .create(true)
                                 .write(true)
                                 .open(result).expect("Unable to open result");
    let mut prev_num: i64 = 0;
    let mut iter = 0;
    let dur = Duration::new(TEN_SEC, 0);
    while iter < LIFE {
        println!("[{:?}]->Write out buffer\n", tid);
        let no_of_det_cals = String::from("/sys/module/dummy/parameters/no_of_det_cals");
        let mut no_of_det_cals = OpenOptions::new()
                                         .read(true)
                                         .open(no_of_det_cals).expect("Unable to read no_of_det_cals");
        thread::sleep(dur);
        /*Reading number of determinant calculations*/
        let end = false;
        let mut det = String::new(); 
        while end == false {
          let mut buffer: [u8; 1] = [0; 1];
          no_of_det_cals.read((&mut buffer)).expect("Unable to read no_of_det_cals");
          let buffer = buffer[0] as char;
          if buffer == '\n' {
            break;
          }
          det.push(buffer);
        }
        let num: i64 = det.parse::<i64>().unwrap();
        println!("[{:?}]->Throughput {}", tid, num - prev_num);
        write!(result, "{}", num - prev_num).expect("Unable to write output");
        prev_num = num;
        iter = iter + 1;
    }
}


fn reader(f: &mut File, tid: &thread::ThreadId) {
    let mut buffer: [u8; MATRIX_LEN * MATRIX_LEN * 4] = [0; MATRIX_LEN * MATRIX_LEN * 4];
    f.read((&mut buffer)).expect("Unable to read");

    let matrix = unsafe {
        mem::transmute::<[u8; MATRIX_LEN * MATRIX_LEN * 4], [i32; MATRIX_LEN * MATRIX_LEN]>(buffer)
    };
}


fn compute(tid: &thread::ThreadId) {
}


fn main() {
    // --snip--
		let no_of_reads = String::from("/sys/module/dummy/parameters/no_of_reads");
    let mut f = OpenOptions::new()
                            .write(true)
                            .open(no_of_reads).expect("Unable to open no_of_reads");
    write!(f, "{}", '0').expect("Unable to reset");

		let dummy = String::from("/dev/dummychar");
    let mut f = OpenOptions::new()
                            .read(true)
                            .open(&dummy).expect("Unable to open dummychar");
    
    // Init measurement thread
    let measurement_thread = thread::spawn(|| {
        measurement(&thread::current().id());
    });

    // Init reader thread
    let reader_thread = thread::spawn(move || {
        reader(&mut f, &thread::current().id());
    });

    // Init compute thread pool
    let pool = ThreadPool::new(N_WORKERS);

    for _ in 0..N_WORKERS {
        pool.execute(move || {
            compute(&thread::current().id());
        });
    }


    let var = -77187305771648_i64;
    let answer = unsafe {
        mem::transmute::<i64, [i8; 8]>(var);
    };

    measurement_thread.join();
    reader_thread.join();

    let mut f = OpenOptions::new()
                            .write(true)
                            .create(true)
                            .open(&dummy).expect("Unable to open dummychar");

    //write!(f, "{:?}", answer).expect("Unable to write");
}

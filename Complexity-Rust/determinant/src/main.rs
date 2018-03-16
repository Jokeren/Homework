extern crate threadpool;

use std::env;
use std::fs::OpenOptions;
use std::fs::File;
use std::io::prelude::*;
use std::io::Cursor;
use std::mem;
use std::thread;
use std::time::Duration;
use std::sync::mpsc::{Sender, Receiver};
use std::sync::mpsc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;

use threadpool::ThreadPool;

const MATRIX_LEN: usize = 32;
const N_WORKERS: usize = 16;
const LIFE: i32 = 1;
const TEN_SEC: u64 = 1;

static mut terminate: AtomicBool = AtomicBool::new(false);

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

    unsafe {
        terminate.store(true, Ordering::Relaxed);
    }
}


fn reader(f: &mut File, read_sems: &mut Vec<Arc<AtomicBool> >, write_sems: &mut Vec<Arc<AtomicBool> >, tid: &thread::ThreadId) {
    loop {
        let is_terminate = unsafe {
            terminate.load(Ordering::Relaxed)
        };
        if is_terminate == true {
            break;
        }

        let mut buffer: [u8; MATRIX_LEN * MATRIX_LEN * 4] = [0; MATRIX_LEN * MATRIX_LEN * 4];
        f.read((&mut buffer)).expect("Unable to read");

        let matrix = unsafe {
            mem::transmute::<[u8; MATRIX_LEN * MATRIX_LEN * 4], [i32; MATRIX_LEN * MATRIX_LEN]>(buffer)
        };

        let value = read_sems[0].store(true, Ordering::Acquire);
    }
}


fn compute(read_sem: &mut Arc<AtomicBool>, write_sem: &mut Arc<AtomicBool>, tid: &thread::ThreadId) {
    println!("[{:?}]->Start compute...", tid);
}


fn writer(f: &mut File, tid: &thread::ThreadId) {
    
}


fn main() {
    let mut read_sems: Vec<Arc<AtomicBool> > = Vec::new();
    let mut write_sems: Vec<Arc<AtomicBool> > = Vec::new();
    let mut compute_threads: Vec<Arc<Option<thread::JoinHandle<_> > > > = Vec::new();

    for i in 0..N_WORKERS {
        read_sems.push(Arc::new(AtomicBool::new(false)));
        write_sems.push(Arc::new(AtomicBool::new(false)));
    }

    // --snip--
		let no_of_reads = String::from("/sys/module/dummy/parameters/no_of_reads");
    let mut f = OpenOptions::new()
                            .write(true)
                            .open(no_of_reads).expect("Unable to open no_of_reads");
    write!(f, "{}", '0').expect("Unable to reset");

		let dummy = String::from("/dev/dummychar");
    let mut f = OpenOptions::new()
                            .read(true)
                            .open(&dummy).expect("Unable to open dummychar for read");
    
    // Init measurement thread
    let measurement_thread = thread::spawn(|| {
        measurement(&thread::current().id());
    });

    // Init compute thread pool
    //let pool:[thread; N_WORKERS] = [threadThreadPool::new(N_WORKERS);

    for i in 0..N_WORKERS {
        let mut write_sem = write_sems[i].clone();
        let mut read_sem = read_sems[i].clone();
        let compute_thread = thread::spawn(move || {
            compute(&mut write_sem, &mut read_sem, &thread::current().id());
        });
        compute_threads.push(Arc::new(Some(compute_thread)));
    }

    // Init reader thread
    let reader_thread = thread::spawn(move || {
        reader(&mut f, &mut read_sems, &mut write_sems, &thread::current().id());
    });

    let mut f = OpenOptions::new()
                            .write(true)
                            .create(true)
                            .open(&dummy).expect("Unable to open dummychar for write");

    //write!(f, "{:?}", answer).expect("Unable to write");

    // Init writer thread
    let writer_thread = thread::spawn(move || {
        writer(&mut f, &thread::current().id());
    });


    let var = -77187305771648_i64;
    let answer = unsafe {
        mem::transmute::<i64, [i8; 8]>(var);
    };

    measurement_thread.join();
    reader_thread.join();
    for i in 0..N_WORKERS {
        let mut t = compute_threads.as_mut().get(i).take().unwrap();
        t.join();
    }

}

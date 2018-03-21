use std::mem;
use std::thread;
use std::fs::OpenOptions;
use std::fs::File;
use std::io::prelude::*;
use std::time::Duration;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::sync::Mutex;

const MATRIX_LEN: usize = 32;
const N_WORKERS: usize = 16;
const NUM_BULKS: usize = 8;
const LIFE: i32 = 18;
const TEN_SEC: u64 = 10;

static mut TERMINATE: AtomicBool = AtomicBool::new(false);
static mut GLOBAL_BUFFER1: [[[u8; MATRIX_LEN * MATRIX_LEN * 4]; NUM_BULKS]; N_WORKERS] = [[[0; MATRIX_LEN * MATRIX_LEN * 4]; NUM_BULKS]; N_WORKERS];
static mut GLOBAL_BUFFER2: [[[u8; MATRIX_LEN * MATRIX_LEN * 4]; NUM_BULKS]; N_WORKERS] = [[[0; MATRIX_LEN * MATRIX_LEN * 4]; NUM_BULKS]; N_WORKERS];
static mut WRITE_VALUES: [[i64; NUM_BULKS]; N_WORKERS] = [[0; NUM_BULKS]; N_WORKERS];


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
    let no_of_det_cals = String::from("/sys/module/dummy/parameters/no_of_det_cals");
    let mut no_of_det_cals = OpenOptions::new()
                                         .read(true)
                                         .open(no_of_det_cals).expect("Unable to read no_of_det_cals");
    thread::sleep(dur);

    // Read no_of_det_cals into a string and parse it into a number
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
    println!("[{:?}]->Write out buffer", tid);
    write!(result, "{}\n", num - prev_num).expect("Unable to write output");
    prev_num = num;
    iter = iter + 1;
  }

  unsafe {
    TERMINATE.store(true, Ordering::Relaxed);
  }
  println!("[{:?}]->End measurement", tid);
}


fn reader(compute_read_sems: &mut Vec<Arc<AtomicBool> >, compute_write_sems: &mut Vec<Arc<AtomicBool> >, f: &mut File, tid: &thread::ThreadId) {
  println!("[{:?}]->Start reading...", tid);
  let mut order = 0;
  loop {
    let is_terminate = unsafe {
      TERMINATE.load(Ordering::Relaxed)
    };
    if is_terminate == true {
      break;
    }

    // Pingpong buffer switch between GLOBAL_BUFFER1 and GLOBAL_BUFFER2
    if order % 2 == 0 {
      unsafe {
        for t in 0..N_WORKERS {
          for b in 0..NUM_BULKS {
            f.read((&mut GLOBAL_BUFFER1[t][b])).expect("Unable to read");
          }
        }
      }
    } else {
      unsafe {
        for t in 0..N_WORKERS {
          for b in 0..NUM_BULKS {
            f.read((&mut GLOBAL_BUFFER2[t][b])).expect("Unable to read");
          }
        }
      }
    }

    for i in 0..N_WORKERS {
      while compute_write_sems[i].compare_and_swap(true, false, Ordering::Acquire) == false {
        let is_terminate = unsafe {
          TERMINATE.load(Ordering::Relaxed)
        };
        if is_terminate == true {
          println!("[{:?}]->End read...", tid);
          return;
        }
      }
      compute_read_sems[i].store(true, Ordering::Release); 
    }

    order = order + 1;
  }
  println!("[{:?}]->End read...", tid);
}


fn determinant(matrix: &mut [f64; MATRIX_LEN * MATRIX_LEN])->i64 {
  let mut det: f64 = 1.0;

  for i in 0..MATRIX_LEN {
    let mut max_loc = i;
    for j in (i + 1)..MATRIX_LEN {
      if matrix[j * MATRIX_LEN + i] != 0.0 {
        max_loc = j;
      }
    }

    if max_loc != i {
      det *= -1.0;
      for j in i..MATRIX_LEN {
        let tmp = matrix[max_loc * MATRIX_LEN + j];
        matrix[max_loc * MATRIX_LEN + j] = matrix[i * MATRIX_LEN + j];
        matrix[i * MATRIX_LEN + j] = tmp;
      }
    }

    for j in (i + 1)..MATRIX_LEN {
      let fraction = matrix[j * MATRIX_LEN + i] / matrix[i * MATRIX_LEN + i];
      for k in i..MATRIX_LEN {
        matrix[j * MATRIX_LEN + k] = matrix[j * MATRIX_LEN + k] - fraction * matrix[i * MATRIX_LEN + k];
      }
    }
    det *= matrix[i * MATRIX_LEN + i];
  }
  return det as i64;
}


fn compute(compute_read_sem: &mut Arc<AtomicBool>,
           compute_write_sem: &mut Arc<AtomicBool>,
           writer_read_sem: &mut Arc<AtomicBool>,
           writer_write_sem: &mut Arc<AtomicBool>,
           tid: &thread::ThreadId,
           index: usize) {
  println!("[{:?}]->Start computing...", tid);
  let mut order = 0;
  let mut compute_buffer: [f64; MATRIX_LEN * MATRIX_LEN] = [0.0; MATRIX_LEN * MATRIX_LEN];
  let mut dets: [i64; NUM_BULKS] = [0; NUM_BULKS];
  loop {
    let is_terminate = unsafe {
      TERMINATE.load(Ordering::Relaxed)
    };
    if is_terminate == true {
      break;
    }

    while compute_read_sem.compare_and_swap(true, false, Ordering::Acquire) == false {
      let is_terminate = unsafe {
        TERMINATE.load(Ordering::Relaxed)
      };
      if is_terminate == true {
        println!("[{:?}]->End computing...", tid);
        return;
      }
    }

    // Pingpong buffer switch between GLOBAL_BUFFER1 and GLOBAL_BUFFER2
    let matrix = unsafe {
      if order % 2 == 0 {
        mem::transmute::<[[u8; MATRIX_LEN * MATRIX_LEN * 4]; NUM_BULKS], [[i32; MATRIX_LEN * MATRIX_LEN]; NUM_BULKS]>(GLOBAL_BUFFER1[index])
      } else {
        mem::transmute::<[[u8; MATRIX_LEN * MATRIX_LEN * 4]; NUM_BULKS], [[i32; MATRIX_LEN * MATRIX_LEN]; NUM_BULKS]>(GLOBAL_BUFFER2[index])
      }
    };

    compute_write_sem.store(true, Ordering::Release);

    // Compute det values
    for i in 0..(NUM_BULKS) {
      for j in 0..(MATRIX_LEN * MATRIX_LEN) {
        compute_buffer[j] = matrix[i][j] as f64;
      }
      dets[i] = determinant(&mut compute_buffer);
    }

    while writer_write_sem.compare_and_swap(true, false, Ordering::Acquire) == false {
      let is_terminate = unsafe {
        TERMINATE.load(Ordering::Relaxed)
      };
      if is_terminate == true {
        println!("[{:?}]->End compute...", tid);
        return;
      }
    }

    unsafe {
      for i in 0..NUM_BULKS {
        WRITE_VALUES[index][i] = dets[i];
      }
    }

    writer_read_sem.store(true, Ordering::Release);

    order = order + 1;
  }
  println!("[{:?}]->End computing...", tid);
}


fn writer(writer_read_sems: &mut Vec<Arc<AtomicBool> >, writer_write_sems: &mut Vec<Arc<AtomicBool> >, f: &mut File, tid: &thread::ThreadId) {
  println!("[{:?}]->Start writing...", tid);

  let mut dets: [i64; NUM_BULKS] = [0; NUM_BULKS];
  let mut order = 0;
  loop {
    let is_terminate = unsafe {
      TERMINATE.load(Ordering::Relaxed)
    };
    if is_terminate == true {
      break;
    }
    while writer_read_sems[order % N_WORKERS].compare_and_swap(true, false, Ordering::Acquire) == false {
      let is_terminate = unsafe {
        TERMINATE.load(Ordering::Relaxed)
      };
      if is_terminate == true {
        println!("[{:?}]->End write...", tid);
        return;
      }
    }
    // Read results
    unsafe {
      for i in 0..NUM_BULKS {
        dets[i] = WRITE_VALUES[order % N_WORKERS][i];
      }
    }
    writer_write_sems[order % N_WORKERS].store(true, Ordering::Release);

    // Write results
    for i in 0..NUM_BULKS {
      let var = dets[i];
      let mut answer = unsafe {
        mem::transmute::<i64, [u8; 8]>(var)
      };
      f.write(&answer).expect("Unable to write");
    }

    // Switch to next compute thread's results
    order = order + 1;
  }
  
  println!("[{:?}]->End writing...", tid);
}


fn main() {
  // Init sems
  let mut compute_read_sems: Vec<Arc<AtomicBool> > = Vec::new();
  let mut compute_write_sems: Vec<Arc<AtomicBool> > = Vec::new();
  let mut writer_read_sems: Vec<Arc<AtomicBool> > = Vec::new();
  let mut writer_write_sems: Vec<Arc<AtomicBool> > = Vec::new();
  let compute_threads = Arc::new(Mutex::new(Some(Vec::new())));

  for _ in 0..N_WORKERS {
    compute_read_sems.push(Arc::new(AtomicBool::new(false)));
    compute_write_sems.push(Arc::new(AtomicBool::new(true)));
    writer_write_sems.push(Arc::new(AtomicBool::new(true)));
    writer_read_sems.push(Arc::new(AtomicBool::new(false)));
  }

  // Reset device
  let no_of_reads = String::from("/sys/module/dummy/parameters/no_of_reads");
  let mut f = OpenOptions::new()
                          .write(true)
                          .open(no_of_reads).expect("Unable to open no_of_reads");
  write!(f, "{}", '0').expect("Unable to reset");

  // Open dummy
  let dummy = String::from("/dev/dummychar");
  let mut f = OpenOptions::new()
                          .read(true)
                          .open(&dummy).expect("Unable to open dummychar for read");

  // Init compute thread pool
  for i in 0..N_WORKERS {
    let mut compute_write_sem = compute_write_sems[i].clone();
    let mut compute_read_sem = compute_read_sems[i].clone();
    let mut writer_write_sem = writer_write_sems[i].clone();
    let mut writer_read_sem = writer_read_sems[i].clone();
    let compute_thread = thread::spawn(move || {
        compute(&mut compute_read_sem, &mut compute_write_sem, &mut writer_read_sem, &mut writer_write_sem, &thread::current().id(), i);
        });
    compute_threads.lock().unwrap().as_mut().unwrap().push(compute_thread);
  }

  // Init reader thread
  let reader_thread = thread::spawn(move || {
      reader(&mut compute_read_sems, &mut compute_write_sems, &mut f, &thread::current().id());
      });

  // Init writer thread
  let mut f = OpenOptions::new()
                          .write(true)
                          .append(true)
                          .open(&dummy).expect("Unable to open dummychar for write");

  let writer_thread = thread::spawn(move || {
      writer(&mut writer_read_sems, &mut writer_write_sems, &mut f, &thread::current().id());
      });

  // Init measurement thread
  let measurement_thread = thread::spawn(|| {
      measurement(&thread::current().id());
      });

  // Join threads
  measurement_thread.join().expect("Unable to join measurement thread");
  reader_thread.join().expect("Unable to reader measurement thread");
  writer_thread.join().expect("Unable to writer measurement thread");
  for t in compute_threads.lock().unwrap().take().unwrap().into_iter() {
    t.join().expect("Unable to join compute thread");
  }
}

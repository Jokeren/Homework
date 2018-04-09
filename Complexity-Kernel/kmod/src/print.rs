//! Common kernel debug print support

extern "C" {
    pub fn printk(fmt: *const u8, ...) -> i32;
}

// Common printing macro
#[macro_export] 
macro_rules! print {
    ($fmt:expr) => ({
        unsafe { ::print::printk(concat!($fmt, "\0").as_ptr()); }
    });
    ($fmt:expr, $($arg:tt)+) => ({
        unsafe { ::print::printk(concat!($fmt, "\0").as_ptr(), $($arg)*); }
    });
}

// Common printing macro including newline
#[macro_export] 
macro_rules! println {
  ($fmt:expr)              => (print!(concat!($fmt, "\n")));
  ($fmt:expr, $($arg:tt)+) => (print!(concat!($fmt, "\n"), $($arg)*));
}

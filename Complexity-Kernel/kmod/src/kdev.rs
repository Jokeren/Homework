//! Common kernel functions

extern "C" {
    pub fn kdev_open() -> i32;
    pub fn kdev_release() -> i32;
    pub fn kdev_read(buf: *mut i32) -> i32;
    pub fn kdev_write(buf: *mut i64, size: i32) -> i32;
    pub fn reset_kdummy() -> i32;
    pub fn get_no_det_cals() -> i32;
}

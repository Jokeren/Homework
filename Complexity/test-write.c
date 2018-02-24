#include "write_back_queue.h"
#include <stdio.h>

int main() {
  write_back_queue_init();
  size_t num = 1000;
  size_t i;
  bool get = false;
  for (i = 0; i < num; ++i) {
    write_back_queue_set_val(i + 1, i);
  }
  for (i = 0; i < num; ++i) {
    size_t val;
    if ((val = write_back_queue_get_val(i + 1, &get)) != i) {
      printf("error! val %zu expected %zu\n", val, i);
    }
  }
  write_back_queue_destory();
}

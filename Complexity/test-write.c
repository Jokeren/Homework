#include "write_back_queue.h"
#include <stdio.h>

int main() {
  write_back_queue_init();
  size_t num = 1000;
  size_t i;
  for (i = 0; i < num; ++i) {
    write_back_queue_set_val(i + 1, i);
  }
  for (i = 0; i < num; ++i) {
    long long val;
    if (write_back_queue_try_get_val(i + 1, &val) == false ||
        val != i) {
      printf("error! val %lld expected %zu\n", val, i);
    }
  }
  write_back_queue_destory();
}

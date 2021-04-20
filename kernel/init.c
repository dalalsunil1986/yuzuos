#include <kernel/utils/log.h>
#include <stdbool.h>

void kernel_init()
{
  log_init();

  while (true)
    ;
}

#include <kernel/utils/log.h>

void kernel_init()
{
  log_init();

  while (1)
    ;
}

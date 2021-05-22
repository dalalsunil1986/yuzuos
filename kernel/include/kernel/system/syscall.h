#pragma once

#include <ukernel/syscall.h>

void syscall_init();
int syscall_brk(void *addr);

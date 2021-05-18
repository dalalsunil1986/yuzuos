#pragma once

#include <ukernel/fcntl.h>

int open(const char *path, int flags, ...);

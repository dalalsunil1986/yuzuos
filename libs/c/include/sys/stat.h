#pragma once

#include <ukernel/stat.h>

int stat(const char *restrict path, struct stat *restrict buf);
int fstat(int fildes, struct stat *buf);

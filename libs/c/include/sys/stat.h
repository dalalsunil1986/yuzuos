#pragma once

#include <ukernel/stat.h>

int fstat(int fildes, struct stat *buf);

#pragma once

#include <stdint.h>
#include <stddef.h>

uint32_t mmap_map(uint32_t addr, size_t len, int fd);

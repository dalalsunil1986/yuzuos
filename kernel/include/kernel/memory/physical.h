#pragma once

#include <stdint.h>

#define PHYS_MM_BLOCK 4096
#define PHYS_MM_BLOCK_P_BYTE 8

void phys_mm_init();
void *phys_mm_block_alloc();
void *phys_mm_block_n_alloc(uint32_t len);
void phys_mm_block_free(void *block);

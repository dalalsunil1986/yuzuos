#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BITMAP_INDEX(bit) ((bit) / 32)
#define BITMAP_OFFSET(bit) (1 << ((bit) % 32))
#define BITMAP_NULL 0xFFFFFFFF
#define BITMAP_BLANK -1

void bitmap_set(uint32_t *bitmap, uint32_t bit);
void bitmap_unset(uint32_t *bitmap, uint32_t bit);
bool bitmap_test(uint32_t *bitmap, uint32_t bit);

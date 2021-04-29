#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BITMAP_INDEX(bit) ((bit) / 32)
#define BITMAP_OFFSET(bit) (1 << ((bit) % 32))
#define BITMAP_NULL 0xFFFFFFFF

void bitmap_set(uint32_t *bitmap, uint32_t bit);
void bitmap_unset(uint32_t *bitmap, uint32_t bit);
bool bitmap_test(uint32_t *bitmap, uint32_t bit);
uint32_t bitmap_get_first(uint32_t *bitmap, uint32_t frames);
uint32_t bitmap_get_n_first(uint32_t *bitmap, uint32_t frames, uint32_t len);

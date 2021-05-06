#pragma once

#include <stdint.h>

void pit_init();
void pit_wait(uint32_t ms);

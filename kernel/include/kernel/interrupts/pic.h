#pragma once

#include <stdint.h>

#define PIC1 0x20
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)

#define PIC2 0xA0
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10

#define ICW4_8086 0x01

#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b

#define PIC_EOI 0x20

void pic_remap(int offset1, int offset2);
void pic_eoi(uint8_t irq);
void pic_mask_set(uint8_t irq);
void pic_mask_unset(uint8_t irq);
uint16_t pic_irq_get(int ocw3);
uint16_t pic_irr_get(void);
uint16_t pic_isr_get(void);

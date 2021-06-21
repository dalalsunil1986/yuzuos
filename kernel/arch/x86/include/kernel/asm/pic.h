/**
 * @file kernel/arch/x86/asm/pic.h
 * @brief 8259 Programmable Interrupt Controller
 * @author Saullo Bretas Silva
 */

#pragma once

#include <stdint.h>

/* IO base address for master PIC */
#define PIC1 0x20
/* IO command address for master PIC */
#define PIC1_COMMAND PIC1
/* IO data address for master PIC */
#define PIC1_DATA (PIC1 + 1)

/* IO base address for slave PIC */
#define PIC2 0xA0
/* IO command address for slave PIC */
#define PIC2_COMMAND PIC2
/* IO data address for slave PIC */
#define PIC2_DATA (PIC2 + 1)

/* ICW4 (not) needed */
#define ICW1_ICW4 0x01
/* Initialization */
#define ICW1_INIT 0x10

/* 8086/88 (MCS-80/85) mode */
#define ICW4_8086 0x01

/* OCW3 irq ready next CMD read */
#define PIC_READ_IRR 0x0a
/* OCW3 irq service next CMD read */
#define PIC_READ_ISR 0x0b

/* End-of-interrupt command code */
#define PIC_EOI 0x20

/**
 * @brief Reinitialize the PIC controllers
 * Giving them specified vector offsets rather than 8h and 70h, as configured by default
 * 
 * @param offset1 Vector offset for master PIC
 * @param offset2 Same for slave PIC
 */
void pic_remap(int offset1, int offset2);

/**
 * @brief Send an end-of-interrupt command
 * @param irq The IRQ number
 */
void pic_eoi(uint8_t irq);

/**
 * @brief Mask an IRQ
 * @param irq The IRQ number
 */
void pic_mask_set(uint8_t irq);

/**
 * @brief Unmask an IRQ
 * @param irq The IRQ number
 */
void pic_mask_unset(uint8_t irq);

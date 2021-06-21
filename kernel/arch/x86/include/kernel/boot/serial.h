/**
 * @file kernel/arch/x86/boot/serial.h
 * @brief x86 early serial
 * @author Saullo Bretas Silva
 */

#pragma once

#define SERIAL_PORT 0x3f8
#define SERIAL_ZEROPAD 1
#define SERIAL_SIGN 2
#define SERIAL_PLUS 4
#define SERIAL_SPACE 8
#define SERIAL_LEFT 16
#define SERIAL_SMALL 32
#define SERIAL_SPECIAL 64

/* Initialize the early serial on #SERIAL_PORT */
void serial_early_init();

/**
 * @brief An kprintf for x86 boot time 
 * @param format The string to be written, can optionally contain embedded format tags
 * that are replaced by the values specified in subsequent additional arguments and formatted as requested
 */
void serial_early_kprintf(const char *format, ...);

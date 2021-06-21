/**
 * @file kernel/arch/x86/kernel/pic.c
 * @brief  8259 Programmable Interrupt Controller code
 * @author Saullo Bretas Silva
 */

#include <kernel/asm/pic.h>
#include <kernel/asm/io.h>

void pic_remap(int offset1, int offset2)
{
  uint8_t mask1 = inb(PIC1_DATA);
  uint8_t mask2 = inb(PIC2_DATA);

  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, offset1);
  io_wait();
  outb(PIC2_DATA, offset2);
  io_wait();
  outb(PIC1_DATA, 4);
  io_wait();
  outb(PIC2_DATA, 2);
  io_wait();

  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  outb(PIC1_DATA, mask1);
  outb(PIC2_DATA, mask2);
}

void pic_mask_set(uint8_t irq)
{
  uint16_t port;

  if (irq < 8)
    port = PIC1_DATA;
  else
  {
    port = PIC2_DATA;
    irq -= 8;
  }

  uint8_t value = inb(port) | (1 << irq);
  outb(port, value);
}

void pic_mask_unset(uint8_t irq)
{
  uint16_t port;

  if (irq < 8)
    port = PIC1_DATA;
  else
  {
    port = PIC2_DATA;
    irq -= 8;
  }

  uint8_t value = inb(port) & ~(1 << irq);
  outb(port, value);
}

void pic_eoi(uint8_t irq)
{
  if (irq >= 8)
    outb(PIC2_COMMAND, PIC_EOI);

  outb(PIC1_COMMAND, PIC_EOI);
}

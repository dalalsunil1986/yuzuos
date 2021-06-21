#include <kernel/drivers/vga.h>
#include <kernel/asm/io.h>
#include <kernel/utils/log.h>
#include <kernel/memory/virtual.h>
#include <stddef.h>

size_t vga_row;
size_t vga_column;
uint8_t vga_color;
uint16_t *vga_buffer;

uint16_t vga_cursor_get()
{
  uint16_t position = 0;
  outb(0x3D4, 0x0F);
  position |= inb(0x3D5);
  outb(0x3D4, 0x0E);
  position |= ((uint16_t)inb(0x3D5)) << 8;
  return position;
}

void vga_cursor_update(size_t x, size_t y)
{
  uint16_t pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_cursor_enable(uint8_t start, uint8_t end)
{
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | start);

  outb(0x3D4, 0x0B);
  outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

void vga_cursor_disable()
{
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
}

void vga_write_char_at(char c, uint8_t color, size_t x, size_t y)
{
  const size_t index = y * VGA_WIDTH + x;
  vga_buffer[index] = vga_entry(c, color);
}

void vga_new_line()
{
  vga_column = 0;
  vga_row++;

  //FIXME 'Scroll' buffer
  if (vga_row >= VGA_HEIGHT)
  {
    vga_column = 0;
    vga_row = 0;
  }
}

void vga_write_char(char ch)
{
  switch (ch)
  {
  case '\n':
    vga_new_line();
    break;
  default:
    vga_write_char_at(ch, vga_color, vga_column++, vga_row);
  }

  if (vga_column >= VGA_WIDTH)
    vga_new_line();

  vga_cursor_update(vga_column, vga_row);
}

void vga_write(const char *buffer, size_t len)
{
  for (size_t i = 0; i < len; i++)
    vga_write_char(buffer[i]);
}

void vga_init()
{
  vga_row = 0;
  vga_column = 0;
  vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  vga_buffer = (uint16_t *)PHYS_TO_VIRT(0xB8000);

  for (size_t y = 0; y < VGA_HEIGHT; y++)
    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
      const size_t index = y * VGA_WIDTH + x;
      vga_buffer[index] = vga_entry(' ', vga_color);
    }

  vga_cursor_update(vga_column, vga_row);

  log_info("VGA: Initialized\n");
}

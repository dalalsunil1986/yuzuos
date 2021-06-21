/**
 * @file kernel/arch/x86/boot/serial.c
 * @brief Early serial implementation
 * @author Saullo Bretas Silva
 */
#include <kernel/boot/serial.h>
#include <kernel/asm/io.h>
#include <stdarg.h>
#include <stddef.h>

/**
 * @brief Write an char on the serial #SERIAL_PORT
 * @param ch The char to be written
 */
void serial_early_write_char(char ch)
{
  while ((inb(SERIAL_PORT + 5) & 0x20) == 0)
    ;

  outb(SERIAL_PORT, ch);
}

/**
 * @brief Write an buffer on the serial #SERIAL_PORT
 * @param buffer A pointer to the buffer to be written
 * @param len The buffer length
 */
void serial_early_write(char *buffer, int len)
{
  for (int i = 0; i < len; i++)
    serial_early_write_char(buffer[i]);
}

/**
 * @brief Determine the length of a fixed-size string
 * @param str A pointer to the string
 * @param maxlen The maximum length to consider
 * @return The string length
 */
size_t serial_early_vsprintf_strnlen(const char *str, size_t maxlen)
{
  const char *p_str = str;
  while (*p_str && maxlen)
  {
    p_str++;
    maxlen--;
  }
  return (p_str - str);
}

/**
 * @brief Checks if the passed character is a decimal digit character
 * @param ch This is the character to be checked
 * @return This function returns 1 if ch is a digit, else it returns 0.
 */
int serial_early_vsprintf_isdigit(int ch)
{
  return (ch >= '0') && (ch <= '9');
}

/**
 * @brief Converts the string to an integer (type int).
 * @param str This is the string representation of an integral number
 * @return This function returns the converted integral number as an int value, If no 
 * valid conversion could be performed, it returns zero
 */
int serial_early_vsprintf_skip_atoi(const char **str)
{
  int i = 0;
  while (serial_early_vsprintf_isdigit(**str))
    i = i * 10 + *((*str)++) - '0';
  return i;
}

/**
 * @brief Format the string numbers
 * @param str This is the array of char elements where the resulting string is to be stored
 * @param num The number to be formated
 * @param base The number base (2...16)
 * @param size The string length
 * @param precision The number precision
 * @param type The format type to be used
 * @return A pointer to the formated string
 */
char *serial_early_vsprintf_number(char *str, long num, int base, int size, int precision, int type)
{
  if (base < 2 || base > 16)
    return NULL;

  /** 
   * locase = 0 or 0x20. 
   * ORing digits or letters with 'locase' produces same digits or (maybe lowercased) letters 
   */
  char locase = (type & SERIAL_SMALL);
  if (type & SERIAL_LEFT)
    type &= ~SERIAL_ZEROPAD;

  char c = (type & SERIAL_ZEROPAD) ? '0' : ' ';
  char sign = 0;

  if (type & SERIAL_SIGN)
  {
    if (num < 0)
    {
      sign = '-';
      num = -num;
      size--;
    }
    else if (type & SERIAL_PLUS)
    {
      sign = '+';
      size--;
    }
    else if (type & SERIAL_SPACE)
    {
      sign = ' ';
      size--;
    }
  }

  if (type & SERIAL_SPECIAL)
  {
    if (base == 16)
      size -= 2;
    else if (base == 8)
      size--;
  }

  int i = 0;
  char tmp[66];
  if (num == 0)
    tmp[i++] = '0';
  else
  {
    /* We use base 8,10 or 16 only, we don't need 'G' */
    const char digits[16] = "0123456789ABCDEF";
    while (num != 0)
    {
      tmp[i++] = (digits[((unsigned long)num) % (unsigned)base] | locase);
      num = ((unsigned long)num) / (unsigned)base;
    }
  }

  if (i > precision)
    precision = i;
  size -= precision;

  if (!(type & (SERIAL_ZEROPAD + SERIAL_LEFT)))
    while (size-- > 0)
      *str++ = ' ';

  if (sign)
    *str++ = sign;

  if (type & SERIAL_SPECIAL)
  {
    if (base == 8)
      *str++ = '0';
    else if (base == 16)
    {
      *str++ = '0';
      *str++ = ('X' | locase);
    }
  }

  if (!(type & SERIAL_LEFT))
    while (size-- > 0)
      *str++ = c;

  while (i < precision--)
    *str++ = '0';

  while (i-- > 0)
    *str++ = tmp[i];

  while (size-- > 0)
    *str++ = ' ';

  return str;
}

/**
 * @brief Sends formatted output to a string using an argument list passed to it.
 * @param str This is the array of char elements where the resulting string is to be stored
 * @param format  This is the C string that contains the text to be written to the str. 
 * It can optionally contain embedded format tags that are replaced by the values specified in subsequent
 * additional arguments and are formatted as requested
 * @return If successful, the total number of characters written is returned, otherwise a negative number is returned.
 */
int serial_early_vsprintf(char *str, const char *format, va_list args)
{
  char *p_str;
  for (p_str = str; *format; ++format)
  {
    if (*format != '%')
    {
      *p_str++ = *format;
      continue;
    }

    /* Flags for vsprintf_number */
    int flags = 0;
  repeat:
    /* Skips first '%' too */
    ++format;
    switch (*format)
    {
    case '-':
      flags |= SERIAL_LEFT;
      goto repeat;
    case '+':
      flags |= SERIAL_PLUS;
      goto repeat;
    case ' ':
      flags |= SERIAL_SPACE;
      goto repeat;
    case '#':
      flags |= SERIAL_SPECIAL;
      goto repeat;
    case '0':
      flags |= SERIAL_ZEROPAD;
      goto repeat;
    }

    /* Width of output field */
    int field_width = -1;
    if (serial_early_vsprintf_isdigit(*format))
      field_width = serial_early_vsprintf_skip_atoi(&format);
    else if (*format == '*')
    {
      ++format;
      field_width = va_arg(args, int);
      if (field_width < 0)
      {
        field_width = -field_width;
        flags |= SERIAL_LEFT;
      }
    }

    /* Minimum # of digits for integers; max number of chars for from string */
    int precision = -1;
    if (*format == '.')
    {
      ++format;
      if (serial_early_vsprintf_isdigit(*format))
        precision = serial_early_vsprintf_skip_atoi(&format);
      else if (*format == '*')
      {
        ++format;
        precision = va_arg(args, int);
      }
      if (precision < 0)
        precision = 0;
    }

    /* 'h', 'l', or 'L' for integer fields */
    int qualifier = -1;
    if (*format == 'h' || *format == 'l' || *format == 'L')
    {
      qualifier = *format;
      ++format;
    }

    /* Default base */
    int base = 10;
    switch (*format)
    {
    case 'c':
      if (!(flags & SERIAL_LEFT))
        while (--field_width > 0)
          *p_str++ = ' ';
      *p_str++ = (unsigned char)va_arg(args, int);
      while (--field_width > 0)
        *p_str++ = ' ';
      continue;

    case 's':
    {
      const char *string = va_arg(args, char *);
      int len = serial_early_vsprintf_strnlen(string, precision);

      if (!(flags & SERIAL_LEFT))
        while (len < field_width--)
          *p_str++ = ' ';
      for (int i = 0; i < len; ++i)
        *p_str++ = *string++;
      while (len < field_width--)
        *p_str++ = ' ';
      continue;
    }

    case 'p':
      if (field_width == -1)
      {
        field_width = 2 * sizeof(void *);
        flags |= SERIAL_ZEROPAD;
      }
      p_str = serial_early_vsprintf_number(p_str, (unsigned long)va_arg(args, void *), 16, field_width, precision, flags);
      continue;

    case 'n':
      if (qualifier == 'l')
      {
        long *ip = va_arg(args, long *);
        *ip = (p_str - str);
      }
      else
      {
        int *ip = va_arg(args, int *);
        *ip = (p_str - str);
      }
      continue;

    case '%':
      *p_str++ = '%';
      continue;

      /* Integer number formats */
    case 'o':
      base = 8;
      break;

    case 'x':
      flags |= SERIAL_SMALL;
      // fall through
    case 'X':
      base = 16;
      break;

    case 'd':
    case 'i':
      flags |= SERIAL_SIGN;
    case 'u':
      break;

    default:
      *p_str++ = '%';
      if (*format)
        *p_str++ = *format;
      else
        --format;
      continue;
    }

    unsigned long num;
    if (qualifier == 'l')
      num = va_arg(args, unsigned long);
    else if (qualifier == 'h')
    {
      num = (unsigned short)va_arg(args, int);
      if (flags & SERIAL_SIGN)
        num = (short)num;
    }
    else if (flags & SERIAL_SIGN)
      num = va_arg(args, int);
    else
      num = va_arg(args, unsigned int);

    p_str = serial_early_vsprintf_number(p_str, num, base, field_width, precision, flags);
  }
  *p_str = '\0';
  return p_str - str;
}

void serial_early_kprintf(const char *format, ...)
{
  char buffer[1024];
  va_list args;

  va_start(args, format);
  int len = serial_early_vsprintf(buffer, format, args);
  va_end(args);

  serial_early_write(buffer, len);
}

void serial_early_init()
{
  outb(SERIAL_PORT + 1, 0x00);
  outb(SERIAL_PORT + 3, 0x80);
  outb(SERIAL_PORT + 0, 0x03);
  outb(SERIAL_PORT + 1, 0x00);
  outb(SERIAL_PORT + 3, 0x03);
  outb(SERIAL_PORT + 2, 0xC7);
  outb(SERIAL_PORT + 4, 0x0B);
  outb(SERIAL_PORT + 4, 0x1E);
  outb(SERIAL_PORT + 0, 0xAE);

  /* Check if serial has the same byte as sent */
  if (inb(SERIAL_PORT + 0) != 0xAE)
    return;

  /* Set it in normal operation mode */
  outb(SERIAL_PORT + 4, 0x0F);
  serial_early_kprintf("Serial early: Initialized\n");
}

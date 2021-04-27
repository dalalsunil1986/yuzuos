#include <kernel/utils/stdio.h>
#include <kernel/utils/compiler.h>
#include <kernel/utils/ctype.h>

int vsnprintf_atoi(const char **string)
{
  int i = 0;
  while (isdigit(**string))
    i = i * 10 + *((*string)++) - '0';
  return i;
}

char *vsnprintf_number(char *buffer, char *end, unsigned long long number, int base, int field, int precision, int flags)
{
  if (base < 2 || base > 36)
    return NULL;

  if (flags & VSNPRINTF_LEFT)
    flags &= ~VSNPRINTF_ZEROPAD;

  char sign = 0;
  if (flags & VSNPRINTF_SIGN)
  {
    if ((signed long long)number < 0)
    {
      sign = '-';
      number = -(signed long long)number;
      field--;
    }
    else if (flags & VSNPRINTF_PLUS)
    {
      sign = '+';
      field--;
    }
    else if (flags & VSNPRINTF_SPACE)
    {
      sign = ' ';
      field--;
    }
  }
  if (flags & VSNPRINTF_SPECIAL)
  {
    if (base == 16)
      field -= 2;
    else if (base == 8)
      field--;
  }

  const char *digits = (flags & VSNPRINTF_LARGE) ? "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" : "0123456789abcdefghijklmnopqrstuvwxyz";
  char tmp[66];
  int i = 0;

  if (number == 0)
    tmp[i++] = '0';
  else
    while (number != 0)
    {
      tmp[i++] = digits[number % base];
      number /= base;
    }

  if (i > precision)
    precision = i;
  field -= precision;

  if (!(flags & (VSNPRINTF_ZEROPAD + VSNPRINTF_LEFT)))
  {
    while (field-- > 0)
    {
      if (buffer < end)
        *buffer = ' ';
      buffer++;
    }
  }

  if (sign)
  {
    if (buffer < end)
      *buffer = sign;
    buffer++;
  }

  if (flags & VSNPRINTF_SPECIAL)
  {
    if (base == 8)
    {
      if (buffer < end)
        *buffer = '0';
      buffer++;
    }
    else if (base == 16)
    {
      if (buffer < end)
        *buffer = '0';
      buffer++;
      if (buffer < end)
        *buffer = digits[33];
      buffer++;
    }
  }

  char c = (flags & VSNPRINTF_ZEROPAD) ? '0' : ' ';
  if (!(flags & VSNPRINTF_LEFT))
  {
    while (field-- > 0)
    {
      if (buffer < end)
        *buffer = c;
      buffer++;
    }
  }

  while (i < precision--)
  {
    if (buffer < end)
      *buffer = '0';
    buffer++;
  }

  while (i-- > 0)
  {
    if (buffer < end)
      *buffer = tmp[i];
    buffer++;
  }

  while (field-- > 0)
  {
    if (buffer < end)
      *buffer = ' ';
    buffer++;
  }

  return buffer;
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
  if (unlikely((int)size < 0))
    return 0;

  char *buffer = str;
  char *end = str + size;

  if (end < str)
  {
    end = ((void *)-1);
    size = end - str;
  }

  for (; *format; format++)
  {
    if (*format != '%')
    {
      if (buffer < end)
        *buffer = *format;
      buffer++;
      continue;
    }

    int flags = 0;

  repeat:
    switch (*(++format))
    {
    case '-':
      flags |= VSNPRINTF_LEFT;
      goto repeat;
    case '+':
      flags |= VSNPRINTF_PLUS;
      goto repeat;
    case ' ':
      flags |= VSNPRINTF_SPACE;
      goto repeat;
    case '#':
      flags |= VSNPRINTF_SPECIAL;
      goto repeat;
    case '0':
      flags |= VSNPRINTF_ZEROPAD;
      goto repeat;
    }

    int field = -1;
    if (isdigit(*format))
      field = vsnprintf_atoi(&format);
    else if (*format == '*')
    {
      format++;
      field = va_arg(ap, int);

      if (field < 0)
      {
        field = -field;
        flags |= VSNPRINTF_LEFT;
      }
    }

    int precision = -1;
    if (*format == '.')
    {
      format++;
      if (isdigit(*format))
        precision = vsnprintf_atoi(&format);
      else if (*format == '*')
      {
        format++;
        precision = va_arg(ap, int);
      }
      if (precision < 0)
        precision = 0;
    }

    int qualifier = -1;
    if (*format == 'h' || *format == 'l' || *format == 'L' || *format == 'Z' || *format == 'z')
    {
      qualifier = *format;
      format++;

      if (qualifier == 'l' && *format == 'l')
      {
        qualifier = 'L';
        format++;
      }
    }

    int base = 10;
    switch (*format)
    {
    case 'c':
      if (!(flags & VSNPRINTF_LEFT))
      {
        while (--field > 0)
        {
          if (buffer < end)
            *buffer = ' ';
          buffer++;
        }
      }

      char ch = (unsigned char)va_arg(ap, int);
      if (buffer < end)
        *buffer = ch;
      buffer++;

      while (--field > 0)
      {
        if (buffer < end)
          *buffer = ' ';
        buffer++;
      }
      continue;

    case 's':
    {
      const char *string = va_arg(ap, char *);
      if ((unsigned long)string < 4096)
        string = "(NULL)";

      const char *p_string = string;
      while (precision-- && *p_string != '\0')
        p_string++;
      int length = p_string - string;

      if (!(flags & VSNPRINTF_LEFT))
      {
        while (length < field--)
        {
          if (buffer < end)
            *buffer = ' ';
          buffer++;
        }
      }

      for (int i = 0; i < length; i++)
      {
        if (buffer < end)
          *buffer = *string;
        buffer++;
        string++;
      }

      while (length < field--)
      {
        if (buffer < end)
          *buffer = ' ';
        buffer++;
      }
      continue;
    }

    case 'p':
      if (field == -1)
      {
        field = 2 * sizeof(void *);
        flags |= VSNPRINTF_ZEROPAD;
      }

      buffer = vsnprintf_number(buffer, end, (unsigned long)va_arg(ap, void *), 16, field, precision, flags);
      continue;

    case 'n':
      if (qualifier == 'l')
      {
        long *ip = va_arg(ap, long *);
        *ip = (buffer - str);
      }
      else if (qualifier == 'Z' || qualifier == 'z')
      {
        size_t *ip = va_arg(ap, size_t *);
        *ip = (buffer - str);
      }
      else
      {
        int *ip = va_arg(ap, int *);
        *ip = (buffer - str);
      }
      continue;

    case '%':
      if (buffer < end)
        *buffer = '%';
      buffer++;
      continue;

    case 'o':
      base = 8;
      break;

    case 'X':
      flags |= VSNPRINTF_LARGE;
      // fall through
    case 'x':
      base = 16;
      break;

    case 'd':
    case 'i':
      flags |= VSNPRINTF_SIGN;
    case 'u':
      break;

    default:
      if (buffer < end)
        *buffer = '%';
      buffer++;

      if (*format)
      {
        if (buffer < end)
          *buffer = *format;
        buffer++;
      }
      else
        format++;
      continue;
    }

    unsigned long long number = 0;
    if (qualifier == 'L')
      number = va_arg(ap, long long);
    else if (qualifier == 'l')
    {
      number = va_arg(ap, unsigned long);
      if (flags & VSNPRINTF_SIGN)
        number = (signed long)number;
    }
    else if (qualifier == 'Z' || qualifier == 'z')
      number = va_arg(ap, size_t);
    else if (qualifier == 'h')
    {
      number = (unsigned short)va_arg(ap, int);
      if (flags & VSNPRINTF_SIGN)
        number = (signed short)number;
    }
    else
    {
      number = va_arg(ap, unsigned int);
      if (flags & VSNPRINTF_SIGN)
        number = (signed int)number;
    }
    buffer = vsnprintf_number(buffer, end, number, base, field, precision, flags);
  }

  if (buffer < end)
    *buffer = '\0';
  else if (size > 0)
    *end = '\0';

  return buffer - str;
}

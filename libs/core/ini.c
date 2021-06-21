#include <core/ini.h>
#include <core/log.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

char *ini_strip(char *string)
{
  char *p_string = string + strlen(string);
  while (p_string > string && isspace((unsigned char)(*--p_string)))
    *p_string = '\0';
  return string;
}

static char *ini_skip(const char *string)
{
  while (*string && isspace((unsigned char)(*string)))
    string++;
  return (char *)string;
}

char *ini_find(const char *string, const char *chars)
{
  int space = 0;
  while (*string && (!chars || !strchr(chars, *string)) && !(space && strchr(INI_INLINE_COMMENT, *string)))
  {
    space = isspace((unsigned char)(*string));
    string++;
  }
  return (char *)string;
}

char *ini_line_get(char *buffer, char *line)
{
  memset(line, 0, INI_LINE_MAX);
  for (uint8_t i = 0; i < INI_LINE_MAX; i++)
  {
    uint8_t ch = *(buffer++);
    if (ch == 0x0D)
      continue;
    if (ch == 0x0A)
      break;
    *(line++) = ch;
  }
  return buffer;
}

int ini_parse(char *buffer, ini_handler_t handler)
{
  char section[INI_SECTION_MAX] = "";
  char prev_name[INI_NAME_MAX] = "";

  char *start;
  char *end;
  char *name;
  char *value;
  int i_line = 0;
  int result = 0;

  char *line = (char *)calloc(INI_LINE_MAX, sizeof(char));
  if (!line)
    exit(EXIT_FAILURE);

  char *p_buffer = buffer;
  while (true)
  {
    if (!*p_buffer)
      break;

    p_buffer = ini_line_get(p_buffer, line);
    if (!*line)
      continue;
    i_line++;

    start = line;
    if (i_line == 1 && (unsigned char)start[0] == 0xEF && (unsigned char)start[1] == 0xBB && (unsigned char)start[2] == 0xBF)
      start += 3;
    start = ini_skip(ini_strip(start));

    if (strchr(INI_START_COMMENT, *start))
      continue;
    else if (*prev_name && *start && start > line)
    {
      log_info("A\n");
      if (!handler(section, prev_name, start) && !result)
        result = i_line;
    }
    else if (*start == '[')
    {
      end = ini_find(start + 1, "]");
      if (*end == ']')
      {
        *end = '\0';
        strncpy(section, start + 1, sizeof(section) - 1);
        *prev_name = '\0';
      }
      else if (!result)
        result = i_line;
    }
    else if (*start)
    {
      end = ini_find(start, "=:");
      if (*end == '=' || *end == ':')
      {
        *end = '\0';
        name = ini_strip(start);
        value = end + 1;
        end = ini_find(value, NULL);
        if (*end)
          *end = '\0';

        value = ini_skip(value);
        ini_strip(value);
        strncpy(prev_name, name, sizeof(prev_name) - 1);
        if (!handler(section, name, value) && !result)
          result = i_line;
      }
      else if (!result)
        result = i_line;
    }
  }
  free(line);
  return result;
}

void ini_open(const char *path, ini_handler_t handler)
{
  int fd = open(path, O_RDONLY);
  if (fd < 0)
    exit(fd);

  struct stat stat;
  int result = fstat(fd, &stat);
  if (result < 0)
    exit(result);

  char *buffer = calloc(stat.st_size, sizeof(char));
  if (!buffer)
    exit(EXIT_FAILURE);

  result = read(fd, buffer, stat.st_size);
  if (result < 0)
    exit(result);

  ini_parse(buffer, handler);
}

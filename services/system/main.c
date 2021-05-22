#include <utils/log.h>
#include <utils/ini.h>
#include <stdbool.h>
#include <unistd.h>

int system_ini_handler(const char *section, const char *key, const char *value)
{
  //FIXME support multiple keys from system.ini (use hashmap!?)
  (void)section;
  (void)key;

  pid_t pid = fork();
  if (!pid)
  {
    char *path = (char *)value;
    char *const argv[] = {path, NULL};
    execv(path, argv);
  }

  return INI_SUCCESS;
}

int main()
{
  ini_open("/etc/system.ini", system_ini_handler);

  while (true)
    ;
}

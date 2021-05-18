#include <stdlib.h>
#include <errno.h>

extern int main(int argc, char *argv[], char *envp[]);

void _start(int argc, char *argv[], char *envp[])
{
  program_invocation_name = argc > 0 ? argv[0] : "";
  //FIXME implement program_invocation_short_name

  int status = main(argc, argv, envp);
  exit(status);
}

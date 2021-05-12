extern int main(int argc, char *argv[], char *envp[]);

void _start(int argc, char *argv[], char *envp[])
{
  int code = main(argc, argv, envp);
  // exit(code);
}

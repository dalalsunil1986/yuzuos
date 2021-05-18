#pragma once

#define EXIT_FAILURE -1
#define EXIT_SUCCESS 0

void abort(void);
int atexit(void (*)(void));
int atoi(const char *);
void free(void *);
char *getenv(const char *);
void *malloc(size_t);
void exit(int status);

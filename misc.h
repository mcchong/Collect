/*
 Copyright (C) 2001 Charles L. A. Clarke.  All rights reserved.
 */

/*
 Tue Jul  6 23:43:07 EDT 1999
 */
#ifndef MISC_INCLUDED
#define MISC_INCLUDED

#include <stdio.h>
#include <sys/types.h>

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned uint32;
typedef long long int64;
typedef unsigned long long uint64;

#define INT16_FMT "%d"
#define UINT16_FMT "%u"
#define INT32_FMT "%d"
#define UINT32_FMT "%u"
#define INT64_FMT "%lld"
#define UINT64_FMT "%llu"

void warning(char *format, ...);
void error(char *format, ...);
void panic(char *format, ...);
void *localMalloc(size_t size);
void *localRealloc(void *memory, size_t size);
void *localFree(void *memory);
char *localStrdup(const char *string);
char *localStrerror(int errnum);
void setProgramName(char *argv0);
char *getProgramName(void);
int split(char *s, char **a, int m);
ssize_t readAll(int fd, void *buf, size_t bufsiz);
ssize_t writeAll(int fd, void *buf, size_t bufsiz);

int sysCall(int result, char *fileName, int lineNumber);
#define SYSCALL(result) (sysCall ((int) (result), __FILE__, __LINE__))

void assertionFailed(char *fileName, int lineNumber);
#ifdef NDEBUG
#define ASSERT(assertion) ((void) 0)
#else
#define ASSERT(assertion)			\
  do {						\
    if (!(assertion))				\
      assertionFailed (__FILE__, __LINE__);	\
  } while (0)
#endif

#endif

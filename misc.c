/*
 Copyright (C) 2001 Charles L. A. Clarke.  All rights reserved.
 */

/*
 Tue Jul  6 23:42:37 EDT 1999
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#include "misc.h"

#define GETLINE_INITIAL_BUFSIZ 256

#ifdef __sun 
#define HAVE_STRERROR 1
#endif

#ifdef __linux 
#define HAVE_STRERROR 1
#endif

static char *programName = (char *) 0;

void warning(char *format, ...) {
	va_list args;

	fflush(stderr);
	if (programName)
		fprintf(stderr, "%s: ", programName);
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fflush(stderr);
}

void error(char *format, ...) {
	va_list args;

	fflush(stderr);
	if (programName)
		fprintf(stderr, "%s: ", programName);
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fflush(stderr);
	exit(1);
}

void panic(char *format, ...) {
	va_list args;

	fflush(stderr);
	if (programName)
		fprintf(stderr, "%s: internal error: ", programName);
	else
		fprintf(stderr, "internal error: ");
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fflush(stderr);
	exit(1);
}

void *localMalloc(size_t size) {
	void *memory;

	if ((memory = malloc(size)))
		return memory;
	else {
		error("Out of memory!\n");
		/*NOTREACHED*/
		return (void *) 0;
	}
}

void *localRealloc(void *memory, size_t size) {
	if ((memory = realloc(memory, size)))
		return memory;
	else {
		error("Out of memory!\n");
		/*NOTREACHED*/
		return (void *) 0;
	}
}

void *localFree(void *memory) {
	if (memory)
		free(memory);
	return (void *) 0;
}

char *localStrdup(const char *string) {
	return strcpy(localMalloc(strlen(string) + 1), string);
}

#ifdef HAVE_STRERROR
char *localStrerror(int errnum) {
	return strerror(errnum);
}
#else
char *localStrerror(int errnum)
{
	extern char *sys_errlist[];

	return sys_errlist[errnum];
}
#endif

void setProgramName(char *argv0) {
	char *pn;

	if (programName)
		localFree(programName);

	if (argv0 == (char *) 0) {
		programName = (char *) 0;
		return;
	}

	for (pn = argv0 + strlen(argv0); pn > argv0; --pn)
		if (*pn == '/') {
			pn++;
			break;
		}

	programName = localStrdup(pn);
}

char *getProgramName(void) {
	return programName;
}

int split(char *s, char **a, int m) {
	int n = 0;

	while (n < m) {
		for (; isspace (*s); s++)
			;
		if (*s == '\0')
			return n;

		a[n++] = s;

		for (s++; *s && !isspace (*s); s++)
			;
		if (*s == '\0')
			return n;

		*s++ = '\0';
	}

	return n;
}

ssize_t readAll(int fd, void *buf, size_t bufsiz) {
	ssize_t current, total = 0;

	while (total < bufsiz) {
		if ((current = read(fd, buf + total, bufsiz - total)) < 0)
			return current;
		if (current == 0)
			return total;
		total += current;
	}

	return bufsiz;
}

ssize_t writeAll(int fd, void *buf, size_t bufsiz) {
	ssize_t current, total = 0;

	while (total < bufsiz) {
		if ((current = write(fd, buf + total, bufsiz - total)) < 0)
			return current;
		total += current;
	}

	return bufsiz;
}

void assertionFailed(char *fileName, int lineNumber) {
	panic("%s:%d\n", fileName, lineNumber);
}

int sysCall(int result, char *fileName, int lineNumber) {
	if (result < 0)
		panic("%s:%d:%s\n", fileName, lineNumber, localStrerror(errno));
	return result;
}

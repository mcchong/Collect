/*
 tcp - Groundwork for basic TCP/IP servers and clients.

 Synopsis:
 cc [ flag ... ] file ...  -lsocket -lnsl [ library ... ]

 #include <stdio.h>
 #include "tcp.h"

 FILE *tcp_accept (unsigned short port);
 FILE * tcp_connect (char *hostName, unsigned short port);
 char *tcp_strerror (void);

 Interface:
 see tcp.h

 Implementation:
 Zombie processes are handed the old-fashioned BSD way, which is
 less "magical".

 We interfere with the caller's signal handling as little as possible.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

static char *errorString = (char *) 0;

char *tcp_strerror(void) {
	if (errorString)
		return errorString;
	else
		return strerror(errno);
}

static void cldHandler(int sig) {
	int status;

	while (wait3(&status, WNOHANG, (struct rusage *) 0) > 0)
		;
}

FILE *tcp_accept(unsigned short port) {
	int s, s0;
	socklen_t addrlen;
	struct sockaddr_in addr;
	int one = 1;
	FILE *fp;

	errorString = (char *) 0;

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return NULL;

	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		return NULL;

	if (setsockopt(s, SOL_TCP, TCP_NODELAY, &one, sizeof(int)) == -1)
		return NULL;

	if (listen(s, 5) == -1)
		return NULL;

	signal(SIGCLD, cldHandler);

	for (;;) {
		addrlen = sizeof(addr);
		while ((s0 = accept(s, (struct sockaddr *) &addr, &addrlen)) == -1)
			if (errno != EINTR) {
				/* a really strange and bad error, so we panic and kill everybody */
				perror(__FILE__);
				kill(0, SIGINT);
				exit(1);
			}

		switch (fork()) {
		case -1:
			while (close(s0) == -1)
				if (errno != EINTR)
					break;
			/* loop slowly */
			sleep(3);
			break;
		case 0:
			while (signal(SIGCLD, SIG_IGN) == SIG_ERR)
				if (errno != EINTR)
					break;
			if ((fp = fdopen(s0, "a+")) != NULL)
				return fp;
			else
				exit(1);
		default:
			while (close(s0) == -1)
				if (errno != EINTR)
					break;
			break;
		}
	}
}

FILE *tcp_connect(char *hostName, unsigned short port) {
	int s;
	int one = 1;
	struct sockaddr_in addr;

	errorString = (char *) 0;

	memset(&addr, '\0', sizeof(addr));
	if ((addr.sin_addr.s_addr = inet_addr(hostName)) == INADDR_NONE) {
		struct hostent *host;

		if ((host = gethostbyname(hostName)) == NULL) {
			errorString = "Invalid host name.";
			return NULL;
		}
		addr.sin_family = host->h_addrtype;
		memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);
	} else
		addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return NULL;

	if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		return NULL;

	if (setsockopt(s, SOL_TCP, TCP_NODELAY, &one, sizeof(int)) == -1)
		return NULL;

	return fdopen(s, "r+");
}

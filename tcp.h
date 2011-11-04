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
 tcp_accept() listens for connections on the designated <port>,
 forking a new process to handle each connection.  A file pointer
 for the connection is returned in the new process.  A NULL pointer
 is returned to indicate an error; tcp_strerror() may be used to
 obtain a description of the error.

 tcp_connect() establishes a connection with a server and returns a
 file pointer for the connection.  The <hostName> argument specifies
 the machine on which the server resides; the <port> argument
 specifies the port on which the server is listening.  The
 <hostName> may be represented in dotted decimal notation or as a
 domain name.  A NULL pointer is returned to indicate an error;
 tcp_strerror() may be used to obtain a description of the error.

 tcp_strerror() returns a description of the error encountered
 during the last call to tcp_accept() or tcp_connect() if a NULL
 pointer was returned from the call.

 Implementation:
 See tcp.c
 */
#ifndef TCP_H_INCLUDED
#define TCP_H_INCLUDED

#include <stdio.h>

char *tcp_strerror(void);
FILE *tcp_accept(unsigned short port);
FILE *tcp_connect(char *hostName, unsigned short port);
#endif

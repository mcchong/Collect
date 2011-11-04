#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "misc.h"
#include "tcp.h"

#define HUGE (64*1024)

char* getHugeLine(FILE *fp) {
	static char buffer[HUGE + 1];
	unsigned nlpos;

	if (fgets(buffer, HUGE + 1, fp) == NULL)
		return (char *) 0;

	nlpos = strlen(buffer) - 1;
	if (buffer[nlpos] == '\n') {
		if (nlpos && buffer[nlpos - 1] == '\r')
			--nlpos;
		buffer[nlpos] = '\0';
		return buffer;
	} else
		return (char *) 0;
}

int main(int argc, char **argv) {
	FILE *client, *file;
	char fileName[100];
	char *line;

	setProgramName(argv[0]);

	client = tcp_accept(8080);

	if ((line = getHugeLine(client))) {
		sprintf(fileName, "logs\/%ld.log", (long) getpid());
		if ((file = fopen(fileName, "a"))) {
			fprintf(file, "%s\n", line);
			fclose(file);
		}
	}

	return 0;
}

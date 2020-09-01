#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void error(char *message) {
	
	fputs(message, stderr);
	exit(EXIT_FAILURE);
}

void usage(char *argv[]) {

	fprintf(stderr, "Usage: %s -f file\n", argv[0]);
	exit(EXIT_FAILURE);
}


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void error(char *message) {
	
	fputs(message, stderr);
	exit(EXIT_FAILURE);
}

void usage(char *argv[]) {

	fprintf(stderr, "Usage: %s -f binary [-c section] [-odslrg]\n", argv[0]);
	exit(EXIT_FAILURE);
}


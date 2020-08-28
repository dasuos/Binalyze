#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "binary.h"
#include "general.h"
#include "handle.h"

int main(int argc, char *argv[]) {

	int option;
	char *file;
	int file_set = false;

	struct Binary *binary;

	while ((option = getopt(argc, argv, ":f:")) != -1) {
		switch (option) {
		case 'f':
			file = optarg;
			file_set = true;
			break;
		case ':':
			fprintf(
				stderr,
				"Option -%c requires an operand\n",
				optopt
			);
			exit(EXIT_FAILURE);
		case '?':
			fprintf(
				stderr,
				"Unrecognized option -%c\n",
				optopt
			);
			exit(EXIT_FAILURE);
		default:
			usage(argv);
		}
	}
	if (!file_set)
		usage(argv);

	binary = parsed_binary(
		validated_handle(
			raw_handle(file)
		),
		file
	);

	printf("Entry point address: %ld\n", binary->entry);
	printf("Binary format:       %s\n", binary->printable_format);
	printf("Binary architecture: %s\n", binary->printable_architecture);
			
	exit(EXIT_SUCCESS);
}


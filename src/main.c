#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "binary.h"
#include "general.h"
#include "handle.h"
#include "symbol.h"

int main(int argc, char *argv[]) {

	int option;
	char *binary;
	bool binary_set = false;
	bool symbols_set = false;

	while ((option = getopt(argc, argv, ":f:s")) != -1) {
		switch (option) {
		case 'f':
			binary = optarg;
			binary_set = true;
			break;
		case 's':
			symbols_set = true;
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
	if (!binary_set)
		usage(argv);

	bfd *handle = validated_handle(raw_handle(binary));
	print_binary(parsed_binary(handle, binary));

	if (symbols_set) {
		struct Symbol *symbols = NULL;
		long count = parsed_symbols(handle, &symbols);
		print_symbols(symbols, count);
	}

	exit(EXIT_SUCCESS);
}

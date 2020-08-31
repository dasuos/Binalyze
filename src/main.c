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
#include "section.h"

int main(int argc, char *argv[]) {

	int option;
	char *binary;

	bool binary_set,
	     symbols_set,
	     dynamic_symbols_set, 
	     sections_set = false;

	struct Symbol *symbols = NULL;
	struct Section *sections = NULL;
	long symbol_count,
	     section_count;

	while ((option = getopt(argc, argv, ":f:osd")) != -1) {
		switch (option) {
		case 'f':
			binary = optarg;
			binary_set = true;
			break;
		case 's':
			symbols_set = true;
			break;
		case 'd':
			dynamic_symbols_set = true;
			break;
		case 'o':
			sections_set = true;
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
		symbol_count = parsed_symbols(handle, &symbols, Static);
		print_symbols(symbols, symbol_count, Static);
	}

	if (dynamic_symbols_set) {
		symbol_count = parsed_symbols(handle, &symbols, Dynamic);
		print_symbols(symbols, symbol_count, Dynamic);
	}

	if (sections_set) {
		section_count = parsed_sections(handle, &sections);
		print_sections(sections, section_count);
	}

	exit(EXIT_SUCCESS);
}


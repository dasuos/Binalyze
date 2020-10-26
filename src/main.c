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
#include "disassembly.h"
#include "gadget.h"

int main(int argc, char *argv[]) {

	int option;
	char *binary, *section;

	bool binary_set,
	     symbols_set,
	     dynamic_symbols_set, 
	     sections_set,
	     section_content_set,
	     linear_disassembly_set,
	     recursive_disassembly_set,
	     gadget_scan_set = false;
		

	struct Symbol *symbols = NULL;
	struct Section *sections = NULL;
	long symbol_count, 
	     section_count;

	while ((option = getopt(argc, argv, ":f:c:odslrg")) != -1) {
		switch (option) {
		case 'f':
			binary = optarg;
			binary_set = true;
			break;
		case 'o':
			symbols_set = true;
			break;
		case 'd':
			dynamic_symbols_set = true;
			break;
		case 's':
			sections_set = true;
			break;
		case 'c':
			section = optarg;
			section_content_set = true;
			break;
		case 'l':
			linear_disassembly_set = true;
			break;
		case 'r':
			recursive_disassembly_set = true;
			break;
		case 'g':
			gadget_scan_set = true;
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

	if (linear_disassembly_set) {
		section_count = parsed_sections(handle, &sections);
		print_linear_disassembly(
			parsed_binary(handle, binary),
			sections, 
			section_count
		);

		exit(EXIT_SUCCESS);
	}

	if (recursive_disassembly_set) {
		section_count = parsed_sections(handle, &sections);
		symbol_count = parsed_symbols(handle, &symbols, Static);
		print_recursive_disassembly(
			parsed_binary(handle, binary),
			sections,
			symbols,
			section_count,
			symbol_count
		);

		exit(EXIT_SUCCESS);
	}

	if (gadget_scan_set) {
		section_count = parsed_sections(handle, &sections);
		print_gadgets(
			parsed_binary(handle, binary),
			sections,
			section_count
		);

		exit(EXIT_SUCCESS);	
	}

	if (section_content_set) {
		section_count = parsed_sections(handle, &sections);
		print_section(
			parsed_section(
				sections,
				section_count,
				section
			)
		);

		exit(EXIT_SUCCESS);
	}

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


#include <bfd.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

#include "symbol.h"
#include "general.h"

long parsed_symbols(bfd *handle, struct Symbol **reference) {

	long count, i, j;

	int bytes = bfd_get_symtab_upper_bound(handle);
	if (bytes > 0) {

		//read .symtab section and allocate symbol table
		asymbol **table = (asymbol**) malloc(bytes);
		if (table == NULL)
			error("No memory can be allocated");
		count = bfd_canonicalize_symtab(handle, table);
		if (count < 0)
			error("Unable to read .symtab section");

		//allocate and initialize Symbol structures
		struct Symbol *symbols = malloc(count * sizeof(struct Symbol));
		if (symbols == NULL)
			error("No memory can be allocated");
		for (i = 0, j = 0; i < count; i++) {
			//initialize function symbol only
			if (table[i]->flags & BSF_FUNCTION) {
				symbols[j].type = Function;
				symbols[j].name = table[i]->name;
				symbols[j].address = bfd_asymbol_value(table[i]);
				j++;
			}
		}
		count = j;
		*reference = symbols;

	} else if (bytes < 0)
		error("Unable to read .symtab section");

	return count;
}

void print_symbols(struct Symbol *symbols, long count) {
	if (count > 0) {
		printf("Static symbol table:\n");
		for (long i = 0; i < count; i++) {
			printf(
				"    %-40s 0x%016jx Function\n", 
				symbols[i].name,
				symbols[i].address
			);
		}
	}
}


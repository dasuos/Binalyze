#include <bfd.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

#include "symbol.h"
#include "general.h"

static int upper_bound(bfd *handle, enum SymbolTable type) {
	switch(type) {
	case Static:
		return bfd_get_symtab_upper_bound(handle);
	case Dynamic:
		return bfd_get_dynamic_symtab_upper_bound(handle);
	default:
		return 0;
	}
}

static long canonicalize(bfd *handle, asymbol **table, enum SymbolTable type) {
	switch(type) {
	case Static:
		return bfd_canonicalize_symtab(handle, table);
	case Dynamic:
		return bfd_canonicalize_dynamic_symtab(handle, table);
	default:
		return 0;
	}
}

long parsed_symbols(
	bfd *handle, 
	struct Symbol **reference,
	enum SymbolTable type
) {

	long count, i, j;

	int bytes = upper_bound(handle, type);
	if (bytes > 0) {

		//read .symtab section and allocate symbol table
		asymbol **table = (asymbol**) malloc(bytes);
		if (table == NULL)
			error("No memory can be allocated\n");
		count = canonicalize(handle, table, type);
		if (count < 0)
			error("Unable to read symbol table\n");

		//allocate and initialize Symbol structures
		struct Symbol *symbols = malloc(count * sizeof(struct Symbol));
		if (symbols == NULL)
			error("No memory can be allocated\n");
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
		error("Unable to read symbol table\n");

	return count;
}

void print_symbols(struct Symbol *symbols, long count, enum SymbolTable type) {
	if (count > 0) {
		printf(
			"%s symbol table:\n", 
			type == Static ? "Static" : "Dynamic"
		);
		for (long i = 0; i < count; i++) {
			printf(
				"    %-35s 0x%016jx Function\n", 
				symbols[i].name,
				symbols[i].address
			);
		}
	}
}


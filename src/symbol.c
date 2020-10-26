#include <bfd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "symbol.h"
#include "general.h"

int upper_bound(bfd *handle, enum SymbolTable table_type) {
	switch(table_type) {
	case Static:
		return bfd_get_symtab_upper_bound(handle);
	case Dynamic:
		return bfd_get_dynamic_symtab_upper_bound(handle);
	default:
		return 0;
	}
}

long canonicalize(
	bfd *handle, 
	asymbol **table, 
	enum SymbolTable table_type
) {
	switch(table_type) {
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
	enum SymbolTable table_type
) {

	long count, i, j;
	enum SymbolType symbol_type;

	int bytes = upper_bound(handle, table_type);
	if (bytes > 0) {

		//read .symtab section and allocate symbol table
		asymbol **table = (asymbol**) malloc(bytes);
		if (table == NULL)
			error("No memory can be allocated\n");
		count = canonicalize(handle, table, table_type);
		if (count < 0)
			error("Unable to read symbol table\n");

		//allocate Symbol structures
		struct Symbol *symbols = malloc(count * sizeof(struct Symbol));
		if (symbols == NULL)
			error("No memory can be allocated\n");

		for (i = 0, j = 0; i < count; i++) {
			
			//allow function and data symbols only
			if (table[i]->flags & BSF_FUNCTION)
				symbol_type = Function;
			else if (table[i]->flags & BSF_LOCAL)
				symbol_type = Local;
			else if (table[i]->flags & BSF_GLOBAL)
				symbol_type = Global;
			else
				continue;
		
			//parse symbol type, name and address
			symbols[j].type = symbol_type;
			symbols[j].name = table[i]->name;
			symbols[j].address = bfd_asymbol_value(table[i]);

			j++;
		}
		
		count = j;
		*reference = symbols;

	} else if (bytes < 0)
		error("Unable to read symbol table\n");

	return count;
}

void print_symbols(
	struct Symbol *symbols, 
	long count, 
	enum SymbolTable table_type
) {

	char *symbol_type;

	if (count > 0) {
		printf(
			"%s symbol table:\n", 
			table_type == Static ? "Static" : "Dynamic"
		);
		for (long i = 0; i < count; i++) {
			switch(symbols[i].type) {
			case Function:
				symbol_type = "Function";
				break;
			case Local:
				symbol_type = "Local";
				break;
			default:
				symbol_type = "Global";
				break;
			}

			printf(
				"    %-40s %-8jx %s\n", 
				symbols[i].name,
				symbols[i].address,
				symbol_type
			);
		}
	}
}


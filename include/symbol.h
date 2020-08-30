#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <bfd.h>

struct Symbol {

	enum Type {Unknown = 0, Function = 1} type;
	char const *name;
	uint64_t address;
};

long parsed_symbols(bfd *handle, struct Symbol **reference);
void print_symbols(struct Symbol *symbols, long count);

#endif


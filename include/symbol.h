#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <bfd.h>

enum SymbolTable {Static = 0, Dynamic = 1};

struct Symbol {

	enum Type {ForbiddenSymbol = 0, Function = 1} type;
	char const *name;
	uint64_t address;
};

long parsed_symbols(
	bfd *handle, 
	struct Symbol **reference,
	enum SymbolTable type
);
void print_symbols(struct Symbol *symbols, long count, enum SymbolTable type);

#endif


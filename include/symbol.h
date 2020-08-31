#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <bfd.h>

enum SymbolTable {Static = 0, Dynamic = 1};
enum SymbolType {Function = 0, Local = 1, Global = 2};

struct Symbol {

	enum SymbolType type;
	char const *name;
	uint64_t address;
};

long parsed_symbols(
	bfd *handle, 
	struct Symbol **reference,
	enum SymbolTable table_type
);
void print_symbols(
	struct Symbol *symbols, 
	long count, 
	enum SymbolTable table_type
);

#endif


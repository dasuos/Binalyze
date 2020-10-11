#ifndef __DISASSEMBLY_H__
#define __DISASSEMBLY_H__

#include <capstone/capstone.h>

#include "binary.h"
#include "section.h"

void print_linear_disassembly(
	struct Binary *binary,
	struct Section *sections, 
	long section_count
);
void print_recursive_disassembly(
	struct Binary *binary,
	struct Section *sections,
	struct Symbol *symbols,
	long section_count,
	long symbol_count
);

#endif


#ifndef __DISASSEMBLY_H__
#define __DISASSEMBLY_H__

#include <capstone/capstone.h>

#include "binary.h"
#include "section.h"
#include "symbol.h"

void open(struct Binary *binary, csh *handle);

size_t disassemble_linearly(
	csh handle, 
	struct Section text, 
	cs_insn **instructions
);

bool disassemble_recursively(
	csh handle,
	struct Section text,
	uint64_t *entry,
	cs_insn *instruction
);

void print_instruction(cs_insn *instruction, size_t length);

void print_instructions(cs_insn *instructions, size_t count);

bool text_contains(struct Section text, uint64_t address);

bool is_valid_instruction(cs_insn *instruction);

bool is_control_flow(cs_insn *instruction);

bool is_unconditional_flow(cs_insn *instruction);

bool is_halt(cs_insn *instruction);

uint64_t control_flow_target(cs_insn *instruction);

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


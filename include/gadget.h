#ifndef __GADGET_H__
#define __GADGET_H__

#include <capstone/capstone.h>

#include "binary.h"
#include "section.h"

#define GADGET_SIZE 75
#define RET_OPCODE 0xc3

struct Gadget {

	char *instruction;
	uint64_t address;
};

struct Gadgets {
	
	struct Gadget *list;
	size_t count;
	size_t size;
};

bool in_range(uint64_t ret, uint64_t address);

bool before_return(uint64_t ret, cs_insn *instruction);

bool invalid_control_flow(cs_insn *instruction);

bool gadget_oversized(size_t size);

char *append_instruction(char *instructions, cs_insn *instruction);

size_t add_gadget(uint64_t address, char *instruction);

void print_gadgets(
	struct Binary *binary,
	struct Section *sections,
	long section_count
);

#endif


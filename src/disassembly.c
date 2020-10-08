#include <stdio.h>
#include <capstone/capstone.h>

#include "general.h"
#include "binary.h"
#include "section.h"

void print_linear_disassembly(
	struct Binary *binary,
	struct Section *sections, 
	long section_count
) {

	csh handle;
	cs_insn *instructions;
	size_t count;

	//get .text section
	struct Section text = parsed_section(
		sections,
		section_count, 
		".text"
	);

	//open Capstone instance and disassemble
	if (cs_open(CS_ARCH_X86, binary->bits == 64 ? CS_MODE_64 : CS_MODE_32, &handle) != CS_ERR_OK)
		error("Failed to disassemble binary\n");

	count = cs_disasm(
		handle,
		text.contents,
		text.size,
		text.virtual_address,
		0,
		&instructions
	);
	if (count <= 0)
		error("Failed to disassemble binary\n");

	//set maximum instruction length
	size_t max = 0;
	for (size_t i = 0; i < count; i++)
		max = instructions[i].size > max 
			? instructions[i].size : max;

	//print instructions
	for (size_t i = 0; i < count; i++) { 
		printf("%-8jx", instructions[i].address);
		for (size_t j = 0; j < max + 1; j++)
			if (j < instructions[i].size)
				printf("%02x ", instructions[i].bytes[j]);
			else
				printf("   ");
		printf(
			"%-6s %s\n", 
			instructions[i].mnemonic, 
			instructions[i].op_str
		);
        }
}


#include <string.h>
#include <stdio.h>
#include <capstone/capstone.h>

#include "general.h"
#include "binary.h"
#include "section.h"
#include "disassembly.h"
#include "gadget.h"

#define GADGET_SIZE 75
#define RET_OPCODE 0xc3
#define INSTRUCTION_SET_SIZE 256

struct Gadgets gadgets = {.list = NULL, .count = 0, .size = 0};

bool in_range(uint64_t ret, uint64_t address) {
	return address >= ret - GADGET_SIZE && address >= 0;
}

bool beyond_return(uint64_t ret, cs_insn *instruction) {
	return instruction->address > ret;
}

bool control_flow_invalid(cs_insn *instruction) {
	return is_control_flow(instruction) && 
		instruction->id != X86_INS_RET;
}

bool gadget_oversized(size_t size) {
	return size > 5;
}

char *append_instruction(char *instructions, cs_insn *instruction) {
	
	char *output;
	
	output = malloc(INSTRUCTION_SET_SIZE);
	if (output == NULL)
		error("No memory can be allocated\n");

	output[0] = '\0';
	
	//append existing instructions
	if (strlen(instructions) > 0) {
		strcat(output, instructions);
		strcat(output, " ");
	}

	//append new instruction
	strcat(output, instruction->mnemonic);
	if (strlen(instruction->op_str) > 0) {
		strcat(output, " ");
		strcat(output, instruction->op_str);
	}
	strcat(output, ";");

	return output;
}

size_t add_gadget(uint64_t address, char *instruction) {

	if (gadgets.count == 0) {

		gadgets.size = 1;
		gadgets.list = malloc(sizeof(struct Gadget));

	} else if (gadgets.size == gadgets.count) {
		
		gadgets.size *= 2;
		gadgets.list = realloc(
			gadgets.list, 
			gadgets.size * sizeof(struct Gadget)
		);
	}
	if (gadgets.list == NULL)
		error("No memory can be allocated\n");
	
	gadgets.list[gadgets.count].address = address;
	gadgets.list[gadgets.count].instruction = instruction;

	return ++gadgets.count;
}

void print_gadgets(
	struct Binary *binary,
	struct Section *sections,
	long section_count
) {
	
	csh handle;
	uint64_t ret, address;
	cs_insn *instruction;
	size_t size = 0;
	size_t count = 0;
	char *instructions;

	//get .text section
	struct Section text = parsed_section(
		sections,
		section_count,
		".text"
	);

	//open Capstone instance with detailed disassemble mode
	open(binary, &handle);
	cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

	//find opcode of all x86 ret instructions
	for (size_t i = 0; i < text.size; i++) {
		if (text.contents[i] == RET_OPCODE) {

			instruction = cs_malloc(handle);
			if (!instruction) {
				cs_close(&handle);
				error("No memory can be allocated\n");
			}

			ret = text.virtual_address + i;

			//disassemble gadget instructions
			for (uint64_t j = ret - 1; in_range(ret, j); j--) {

				address = j;
				size = 0;
				instructions = "";

				while (disassemble_recursively(handle, text, &address, instruction)) {

					if (!is_valid_instruction(instruction))
				       		break;
					if (beyond_return(ret, instruction))
						break;
					if (control_flow_invalid(instruction))
						break;
					if (gadget_oversized(++size))
						break;

					instructions = append_instruction(
						instructions,
						instruction
					);

					if (instruction->address == ret)
						count = add_gadget(
							address - 1, 
							instructions
						);
				}
			}
		}
	}

	for (size_t i = 0; i < count; i++)
		printf(
			"%-8jx %s\n", 
			gadgets.list[i].address,
			gadgets.list[i].instruction
		);
}


#include <stdint.h>
#include <stdio.h>
#include <capstone/capstone.h>

#include "general.h"
#include "binary.h"
#include "section.h"
#include "symbol.h"
#include "entry.h"

static void open(struct Binary *binary, csh *handle) {

	//open Capstone instance
	if (cs_open(CS_ARCH_X86, binary->bits == 64 ? CS_MODE_64 : CS_MODE_32, handle) != CS_ERR_OK)
		error("Failed to disassemble binary\n");
}

static size_t disassemble_linearly(
	csh handle, 
	struct Section text,
	cs_insn **instructions
) {

	return cs_disasm(
		handle,
		text.contents,
		text.size,
		text.virtual_address,
		0,
		instructions
	);
}

static bool disassemble_recursively(
        csh handle,
        struct Section text,
        uint64_t *entry,
        cs_insn *instruction
) {

	uint64_t offset = *entry - text.virtual_address;
	const uint8_t *code = text.contents + offset;
	size_t code_size = text.size - offset;

	return cs_disasm_iter(
		handle,
		&code,
		&code_size,
		entry,
		instruction
	);
}


static void print_instruction(cs_insn *instruction, size_t length) {

	//print single instruction
	printf("%-8jx", instruction->address);
		for (size_t i = 0; i < length; i++)
			if (i < instruction->size)
				printf("%02x ", instruction->bytes[i]);
			else
				printf("   ");
		printf(
			"%-6s %s\n",
			instruction->mnemonic,
			instruction->op_str
		);
}

static void print_instructions(cs_insn *instructions, size_t count) {

	//set maximum instruction length
	size_t length = 0;
	for (size_t i = 0; i < count; i++)
		length = instructions[i].size > length
			? instructions[i].size : length;

	//print instructions
	for (size_t i = 0; i < count; i++)
		print_instruction(&instructions[i], length + 1);
}

static bool text_contains(struct Section text, uint64_t address) {
	return (text.virtual_address <= address) && 
		(address <= text.virtual_address + text.size);
}

static bool is_valid_instruction(cs_insn *instruction) {
	return (instruction->id != X86_INS_INVALID) && 
		(instruction->size != 0);
}

static bool is_control_flow(cs_insn *instruction) {

	for (size_t i = 0; i < instruction->detail->groups_count; i++)
		switch (instruction->detail->groups[i]) {
		case CS_GRP_JUMP:
		case CS_GRP_CALL:
		case CS_GRP_RET:
		case CS_GRP_IRET:
			return true;
		}
	return false;
}

static bool is_unconditional_flow(cs_insn *instruction) {
	
	switch(instruction->id) {
	case X86_INS_JMP:
	case X86_INS_LJMP:
	case X86_INS_RET:
	case X86_INS_RETF:
	case X86_INS_RETFQ:
		return true;
	default:
		return false;
	}
}

static bool is_halt(cs_insn *instruction) {
	return instruction->id == X86_INS_HLT;
}

static uint64_t control_flow_target(cs_insn *instruction) {
	
	cs_x86_op *operand;

	if (is_control_flow(instruction)) {
		for (size_t i = 0; i < instruction->detail->x86.op_count; i++) {
			operand = &instruction->detail->x86.operands[i];
			if (operand->type == X86_OP_IMM)
				return operand->imm;
		}
	}
	return 0;
}

void print_linear_disassembly(
	struct Binary *binary,
	struct Section *sections, 
	long section_count
) {

	csh handle;
	cs_insn *instructions;

	//open Capstone instance
	open(binary, &handle);

	//get .text section, disassemble and print instructions linearly
	size_t count = disassemble_linearly(
		handle,
		parsed_section(sections, section_count, ".text"),
		&instructions
	);

	if (count <= 0)
		error("Failed to disassemble binary\n");
	print_instructions(instructions, count);
}

void print_recursive_disassembly(
	struct Binary *binary,
	struct Section *sections,
	struct Symbol *symbols,
	long section_count,
	long symbol_count
) {
	
	csh handle;
	cs_insn *instruction;
	uint64_t entry, target;

	//open Capstone instance with detailed disassemble mode
	open(binary, &handle);
	cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

	//get .text section
	struct Section text = parsed_section(
		sections,
		section_count,
		".text"
	);

	instruction = cs_malloc(handle);
	if (!instruction) {
		cs_close(&handle);
		error("No memory can be allocated\n");
	}

	//add main entry point
	if (text_contains(text, binary->entry))
		push_entry(binary->entry);

	//add function symbol entry points
	for (long i = 0; i < symbol_count; i++)
		if (symbols[i].type == Function && text_contains(text, symbols[i].address))
			push_entry(symbols[i].address);

	while (!entries_empty()) {
		entry = pop_entry();

		if (!entry_examined(entry)) {

			//disassemble and print instructions recursively
			while (disassemble_recursively(handle, text, &entry, instruction)) {
				if (is_valid_instruction(instruction)) {

					examine_entry(entry);
					print_instruction(instruction, 15);
					/*
					 * add control flow targets and skip
					 * unconditional flows and halt
					 */
					if (is_halt(instruction))
						break;
					if (is_control_flow(instruction)) {
						target = control_flow_target(
							instruction
						);
						if (target && !entry_examined(target) && text_contains(text, target)) {
							push_entry(target);
							printf("\nTarget examined at address %jx\n\n", target);
						}
						if (is_unconditional_flow(instruction))
							break;
					}
				}
			}
			printf("__________\n\n");
		}
	}
}


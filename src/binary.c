#include <bfd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "binary.h"
#include "general.h"

struct Binary *parsed_binary(bfd *handle, char *file) {

	struct Binary *binary = malloc(sizeof(struct Binary));
	
	//parse binary file name, entry point address
	binary->file = file;
	binary->entry = bfd_get_start_address(handle);

	//parse binary format
	binary->printable_format = handle->xvec->name;
	switch(handle->xvec->flavour) {
	case bfd_target_elf_flavour:
		binary->format = Elf;
		break;
	case bfd_target_coff_flavour:
		binary->format = Pe;
		break;
	case bfd_target_unknown_flavour:
	default:
		error("Unknown binary type\n");
	}

	//parse binary architecture
	const bfd_arch_info_type *architecture = bfd_get_arch_info(handle);
	binary->printable_architecture = architecture->printable_name;
	switch(architecture->mach) {
	case bfd_mach_i386_i386:
		binary->architecture = X86;
		binary->bits = 32;
		break;
	case bfd_mach_x86_64:
		binary->architecture = X86;
		binary->bits = 64;
		break;
	default:
		error("Unknown architecture\n");
	}

	return binary;
}

void print_binary(struct Binary *binary) {
	printf("Entry point address: %ld\n", binary->entry);
	printf("Binary format:       %s\n", binary->printable_format);
	printf("Binary architecture: %s\n", binary->printable_architecture);
}


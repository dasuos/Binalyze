#ifndef __BINARY_H__
#define __BINARY_H__

#include <bfd.h>

struct Binary {

	char *file;
	enum Format {Elf = 1} format;
	char *printable_format;
	enum Architecture {X86 = 1} architecture;
	char const *printable_architecture;
	unsigned int bits;
	uint64_t entry;
};

struct Binary *parsed_binary(bfd *handle, char *file);

void print_binary(struct Binary *binary);

#endif


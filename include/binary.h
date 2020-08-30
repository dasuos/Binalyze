#ifndef __BINARY_H__
#define __BINARY_H__

#include <bfd.h>

struct Binary {

	char *file;
	enum Format {Auto = 0, Elf = 1, Pe = 2} format;
	char *printable_format;
	enum Architecture {None = 0, X86 = 1} architecture;
	char const *printable_architecture;
	unsigned int bits;
	uint64_t entry;
};

struct Binary *parsed_binary(bfd *handle, char *file);
void print_binary(struct Binary *binary);

#endif


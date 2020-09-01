#ifndef __SECTION_H__
#define __SECTION_H__

#include <bfd.h>

enum SectionType {Code = 1, Data = 2};

struct Section {

	enum SectionType type;
	char const *name;
	uint64_t virtual_address;
	uint64_t size;
	uint8_t *contents;
};

long parsed_sections(bfd *handle, struct Section **reference);
void print_sections(struct Section *sections, long count);
struct Section parsed_section(
	struct Section *sections, 
	long count,
	char *name
);
void print_section(struct Section section);

#endif


#include <bfd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "section.h"
#include "general.h"

static void print_ascii(char character) {
	printf("%c", character >= ' ' && character <= '~' ? character : '.');
}

long parsed_sections(bfd *handle, struct Section **reference) {

	asection* list;
	int flags;
	long count = 0;
	enum SectionType type;
	bool read;
	
	//allocate first Section structure
	struct Section *sections = malloc(sizeof(struct Section));
	if (sections == NULL)
		error("No memory can be allocated\n");

	for (list = handle->sections; list; list = list->next) {
		flags = bfd_get_section_flags(handle, list);
		
		//allow code and data sections only
		if (flags & SEC_CODE)
			type = Code;
		else if (flags & SEC_DATA)
			type = Data;
		else 
			continue;

		//allocate next Section structure
		if (count > 0) {
			sections = realloc(
				sections,
				(count + 1) * sizeof(struct Section)
			);
			if (sections == NULL)
				error("No memory can be allocated\n");
		}

		//parse section type, name, virtual address and size
		sections[count].type = type;
		sections[count].name = bfd_section_name(handle, list);
		sections[count].virtual_address = bfd_section_vma(handle, list);
		sections[count].size = bfd_section_size(handle, list);

		//parse section content
		sections[count].contents = (uint8_t*) malloc(
			sections[count].size
		);
		if (sections[count].contents == NULL)
			error("No memory can be allocated\n");
		read = bfd_get_section_contents(
			handle,
			list,
			sections[count].contents,
			0,
			sections[count].size
		);
		if (read == false)
			error("Unable to read section\n");
		count++;
	}
	
	*reference = sections;
	return count;
}

void print_sections(struct Section *sections, long count) {
	if (count > 0) {
		printf("Sections:\n");
		for (long i = 0; i < count; i++)
			printf(
				"    %-40s 0x%016jx %s %-8ju\n",
				sections[i].name,
				sections[i].virtual_address,
				sections[i].type == Data ? "Data" : "Code",
				sections[i].size
			);
	}
}

struct Section parsed_section(
	struct Section *sections, 
	long count,
	char *name
) {

	struct Section section;
	bool found = false;

	//find a section by name and return its content
	for (long i = 0; i < count; i++)
		if (strcmp(sections[i].name, name) == 0) {
			section = (struct Section) sections[i];
			found = true;
		}
	if (!found)
		error("Section not found\n");
	return section;
}

void print_section(struct Section section) {
	
	uint64_t address = section.virtual_address;
	int j;

	printf(
		"%s 0x%016jx %s %ju\n",
		section.name,
		section.virtual_address,
		section.type == Data ? "Data" : "Code",
		section.size
	);

	for (int i = 0; i < section.size; i++) {
		//print ASCII values of ending line
		if ((i % 16) == 0 && i != 0)
			for (j = (i - 16); j < i; j++)
				print_ascii(section.contents[j]);
		//print address of new line
		if ((i % 16) == 0) {
			printf("\n%08jx: ", address);
			address += 16;		
		}
		//print hexadecimal characters
		printf("%02x", section.contents[i]);
		if ((i % 2) == 1)
			printf(" ");
	}
	putchar('\n');
}


#include <bfd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "section.h"
#include "general.h"

long parsed_sections(bfd *handle, struct Section **reference) {

	asection* list;
	int flags;
	long count = 0;
	enum SectionType type;
	bool read;
	
	//allocate first Section structure
	struct Section *sections = malloc(sizeof(struct Section));
	if (sections == NULL)
		error("No memory can be allocated");

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
				error("No memory can be allocated");
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
			error("No memory can be allocated");
		read = bfd_get_section_contents(
			handle,
			list,
			sections[count].contents,
			0,
			sections[count].size
		);
		if (read == false)
			error("Unable to read section");
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
				"    %-40s 0x%016jx %-8ju %s\n",
				sections[i].name,
				sections[i].virtual_address,
				sections[i].size,
				sections[i].type == Data ? "Data" : "Code"
			);
	}
}


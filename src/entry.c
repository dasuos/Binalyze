#include <stdio.h>
#include <stdlib.h>

#include "general.h"
#include "entry.h"

struct Entries entries = {.front = NULL, .rear = NULL};
struct ExaminedEntries seen_entries = {.list = NULL, .count = 0, .size = 0};

void push_entry(uint64_t address) {

	struct Entry *entry = malloc(sizeof(struct Entry));
	if (entry == NULL)
		error("No memory can be allocated\n");

	entry->address = address;
	entry->next = NULL;

	if (entries.rear == NULL) {
		entries.front = entry;
		entries.rear = entry;
	} else {
		entries.rear->next = entry;
		entries.rear = entry;
	}
}

uint64_t pop_entry() {

	if (entries.front == NULL)
		error("Entry is already empty\n");
	
	//pop entry and save entry address
	struct Entry *entry  = entries.front;
	
	entries.front = entries.front->next;
	if (entries.front == NULL)
		entries.rear = NULL;
	uint64_t address = entry->address;

	free(entry);
	
	return address;
}

bool entries_empty() {

	return entries.front == NULL;
}

void examine_entry(uint64_t address) {
        
	if (seen_entries.count == 0) {
		
		seen_entries.size = 1;
		seen_entries.list = malloc(sizeof(uint64_t));
		if (seen_entries.list == NULL)
			error("No memory can be allocated\n");
	
	} else if (seen_entries.size == seen_entries.count) {
		
		seen_entries.size *= 2;
		seen_entries.list = realloc(
			seen_entries.list,
			seen_entries.size * sizeof(uint64_t)
		);
		if (seen_entries.list == NULL)
			error("No memory can be allocated\n");
	}
	seen_entries.list[seen_entries.count++] = address;
}

bool entry_examined(uint64_t address) {

	if (seen_entries.count > 0)
		for (size_t i = 0; i <= seen_entries.count; i++)
			if (seen_entries.list[i] == address)
				return true;
	return false;
}


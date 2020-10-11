#ifndef __ENTRY_H__
#define __ENTRY_H__

#include <stdbool.h>
#include <stdint.h>

struct Entry {
	
	uint64_t address;
	struct Entry *next;
};

struct Entries {
	
	struct Entry *front;
	struct Entry *rear;
};

struct ExaminedEntries {

	uint64_t *list;
	size_t count;
	size_t size;
};


void push_entry(uint64_t address);
uint64_t pop_entry();
bool entries_empty();
void examine_entry(uint64_t address);
bool entry_examined(uint64_t address);

#endif


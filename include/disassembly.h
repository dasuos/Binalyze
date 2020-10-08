#ifndef __DISASSEMBLY_H__
#define __DISASSEMBLY_H__

#include  <capstone/capstone.h>

#include "section.h"

void print_linear_disassembly(struct Section *sections, long section_count);

#endif


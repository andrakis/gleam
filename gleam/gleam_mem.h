// This is probably horrible. In EMCAScript, we used an object. Here, memory
// will be a linked list of memory segments, requiring traversal through the
// list to find the right segment, and testing the offset to see if we need
// to realloc...

#ifndef _GLEAM_MEM_H
#define _GLEAM_MEM_H

#include "gleam_emu.h"

#define PAGE_SIZE 0x10000

struct mem_node {
	gnum start;
	gnum range;
	char *page;
	struct mem_node *prev, *next;
};
typedef struct mem_node mem_node_t;

int mem_init(void);
gnum mem_test(void);
void mem_dump(void);

#endif

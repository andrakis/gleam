// This is probably horrible. In EMCAScript, we used an object. Here, memory
// will be a linked list of memory segments, requiring traversal through the
// list to find the right segment, and testing the offset to see if we need
// to realloc...

#ifndef _GLEAM_MEM_H
#define _GLEAM_MEM_H

#include "gleam.h"

#define PAGE_SIZE 0x10000

struct mem_node {
	gnum start;
	gnum range;
	char *page;
	struct mem_node *prev, *next;
};
typedef struct mem_node mem_node_t;

struct gleam_mem {
	mem_node_t base;
	mem_node_t *head;
};
typedef struct gleam_mem gleam_mem_t;

gleam_mem_t *mem_new(void);
void mem_free(gleam_mem_t *);
gnum mem_test(gleam_mem_t *);
void mem_dump(gleam_mem_t *);
gnum mem_read(gnum location, gleam_mem_t *mem);
void mem_write(gnum location, gnum value, gleam_mem_t *mem);

#endif

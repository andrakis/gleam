/**
 * Gleam memory interface
 *
 * Provides a very large amount of memory to write to and read from, using
 * paging to keep actual usage low.
 *
 * This implements an equivalent to the Cumulative memory scheme which was
 * simply a hashtable.
 * Since we're only using numbers, I figure this will be a much more efficient
 * implementation - pages are allocated in PAGE_SIZE bytes. Unused pages are
 * not allocated.
 * The first time memory is read or written to in a page that does not exist,
 * it is allocated.
 *
 */

#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include "gleam_mem.h"

// First page
static mem_node_t base;

int mem_init ()
{
	base.start = 0;
	base.range = PAGE_SIZE;
	base.next = base.prev = 0;
	base.page = (char*)malloc(PAGE_SIZE);
	if (!base.page) return 1;

	memset(base.page, 0, PAGE_SIZE);
	return 0;
}

void mem_dump () {
	mem_node_t *it = &base;
	gnum pages = 0;

	// Find lowest value
	while(it->prev) it = it->prev;

	// Walk!
	do {
		pages++;
		printf("Page %lld: %lld - %lld\n", pages, it->start, it->start + it->range);
		it = it->next;
	} while(it);

	printf("Total pages: %lld", pages);
}

// I'm sure there is a better way to do this...
gnum mem_align(gnum offset) 
{
	long sign;

	if (offset < 0) {
		sign = -1;
		offset = -offset;
	} else sign = 0;

	if (offset % PAGE_SIZE !=0)
		offset -= offset % PAGE_SIZE;

	if (sign)
		return -offset - PAGE_SIZE;
	else
		return offset;
}

// Needs to align start to PAGE_SIZE!
void mem_allocate(gnum location, mem_node_t *dest) {
	mem_node_t *new_node = (mem_node_t*)malloc(sizeof(mem_node_t));

	new_node->start = mem_align(location);
	new_node->range = PAGE_SIZE;
	new_node->page = (char*)malloc(PAGE_SIZE);
	memset(new_node->page, 0, PAGE_SIZE);

	if (location < dest->start) {
		new_node->next = dest;
		new_node->prev = dest->prev;
		dest->prev = new_node;
	} else {
		new_node->next = dest->next;
		new_node->prev = dest;
		dest->next = new_node;
	}
}

#define READ 0
#define WRITE 1

mem_node_t *mem_select (gnum location, mem_node_t *from) {
	if (location >= from->start && location <= from->start + from->range) {
		return from;
	} else if (location < from->start) {
		// Prev page not exist or too low? Insert page
		if (from->prev == 0 || location > (from->prev->start + from->prev->range)) {
			mem_allocate(location, from);
		} 

		return mem_select(location, from->prev);
	} else {
		// Next page not exist or too high? Insert page
		if (from->next == 0 || location < from->next->start) {
			mem_allocate(location, from);
		}

		return mem_select(location, from->next);
	}
}

gnum mem_read_from(gnum location, mem_node_t *start) {
	register gnum offset = 0;
	gnum page_offset;
	
	gnum dst;
	void *b = &dst;
	gnum loc = location * sizeof(gnum);

	mem_node_t *page = mem_select(loc, start);

	for(offset = 0; offset < sizeof(gnum); offset++) {
		page_offset = (loc - page->start) + offset;
		*((char*)b + offset) = *(page->page + page_offset);
		// Make sure we're in bounds
		page = mem_select(loc, page);
	}

	return dst;
}

gnum mem_read(gnum location) {
	return mem_read_from(location, &base);
}

void mem_write_from(gnum location, gnum value, mem_node_t *start) {
	register gnum offset = 0;
	gnum page_offset;
	
	gnum dst = value;
	void *b = &dst;
	gnum loc = location * sizeof(gnum);

	mem_node_t *page = mem_select(loc, start);

	for(offset = 0; offset < sizeof(gnum); offset++) {
		page_offset = (loc - page->start) + offset;
		*(page->page + page_offset) = *((char*)b + offset);
		// Make sure we're in bounds
		page = mem_select(loc, page);
	}
}

void mem_write(gnum location, gnum value) {
	mem_write_from(location, value, &base);
}

gnum mem_test () {
	gnum offset = 0xFF;

	while(offset < 0xFFFFFFFF) {
		mem_write(offset + 0, 'H');
		mem_write(offset + 1, 'e');
		mem_write(offset + 2, 'l');
		mem_write(offset + 3, 'l');
		mem_write(offset + 4, 'o');

		if (mem_read(offset + 0) != 'H' ||
			mem_read(offset + 1) != 'e' ||
			mem_read(offset + 2) != 'l' ||
			mem_read(offset + 3) != 'l' ||
			mem_read(offset + 4) != 'o') {
				return offset;
		}

		offset *= 10;
	}

	offset = -0xFF;
	while(offset > (gnum)-0xFFFFFF) {
		mem_write(offset + 0, 'H');
		mem_write(offset + 1, 'e');
		mem_write(offset + 2, 'l');
		mem_write(offset + 3, 'l');
		mem_write(offset + 4, 'o');

		if (mem_read(offset + 0) != 'H' ||
			mem_read(offset + 1) != 'e' ||
			mem_read(offset + 2) != 'l' ||
			mem_read(offset + 3) != 'l' ||
			mem_read(offset + 4) != 'o') {
				return offset;
		}

		offset *= 10;
	}

	return 0;
}

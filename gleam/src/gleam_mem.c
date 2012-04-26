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

gleam_mem_t *mem_new ()
{
	gleam_mem_t *mem = (gleam_mem_t*)malloc(sizeof(gleam_mem_t));
	if (!mem) return 0;

	mem->head = &mem->base;
	mem->base.start = 0;
	mem->base.range = PAGE_SIZE;
	mem->base.next = mem->base.prev = 0;
	mem->base.page = (char*)malloc(PAGE_SIZE);
	if (!mem->base.page) {
		free(mem);
		return 0;
	}

	memset(mem->base.page, 0, PAGE_SIZE);
	return mem;
}

void mem_free(gleam_mem_t *mem) {
	// Walk from lowest to highest
	mem_node_t *it = &mem->base;
	mem_node_t *next;

	// Find lowest value
	while(it->prev) it = it->prev;

	// Walk!
	do {
		next = it->next;
		free(it->page);
		free(it);
		it = next;
	} while(it);

	// For some reason, freeing mem resuls in a core dump
	//free(mem);
}

void mem_dump (gleam_mem_t *src) {
	mem_node_t *it = &src->base;
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

gnum mem_read(gnum location, gleam_mem_t *mem) {
	register gnum offset = 0;
	gnum page_offset;
	
	gnum dst;
	void *b = &dst;
	gnum loc = location * sizeof(gnum);

	mem->head = mem_select(loc, mem->head);

	for(offset = 0; offset < sizeof(gnum); offset++) {
		page_offset = (loc - mem->head->start) + offset;
		*((char*)b + offset) = *(mem->head->page + page_offset);
		// Make sure we're in bounds
		mem->head = mem_select(loc, mem->head);
	}

	return dst;
}

void mem_write(gnum location, gnum value, gleam_mem_t *mem) {
	register gnum offset = 0;
	gnum page_offset;
	
	gnum dst = value;
	void *b = &dst;
	gnum loc = location * sizeof(gnum);

	mem->head = mem_select(loc, mem->head);

	for(offset = 0; offset < sizeof(gnum); offset++) {
		page_offset = (loc - mem->head->start) + offset;
		*(mem->head->page + page_offset) = *((char*)b + offset);
		// Make sure we're in bounds
		mem->head = mem_select(loc, mem->head);
	}
}

gnum mem_test (gleam_mem_t *mem) {
	gnum offset = 0xFF;

	while(offset < 0xFFFFFFFF) {
		mem_write(offset + 0, 'H', mem);
		mem_write(offset + 1, 'e', mem);
		mem_write(offset + 2, 'l', mem);
		mem_write(offset + 3, 'l', mem);
		mem_write(offset + 4, 'o', mem);

		if (mem_read(offset + 0, mem) != 'H' ||
			mem_read(offset + 1, mem) != 'e' ||
			mem_read(offset + 2, mem) != 'l' ||
			mem_read(offset + 3, mem) != 'l' ||
			mem_read(offset + 4, mem) != 'o') {
				return offset;
		}

		offset *= 10;
	}

	offset = -0xFF;
	while(offset > (gnum)-0xFFFFFF) {
		mem_write(offset + 0, 'H', mem);
		mem_write(offset + 1, 'e', mem);
		mem_write(offset + 2, 'l', mem);
		mem_write(offset + 3, 'l', mem);
		mem_write(offset + 4, 'o', mem);

		if (mem_read(offset + 0, mem) != 'H' ||
			mem_read(offset + 1, mem) != 'e' ||
			mem_read(offset + 2, mem) != 'l' ||
			mem_read(offset + 3, mem) != 'l' ||
			mem_read(offset + 4, mem) != 'o') {
				return offset;
		}

		offset *= 10;
	}

	return 0;
}

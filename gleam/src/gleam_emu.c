/**
 * Gleam emulator
 *
 * C implemtation of the Cumulative single-op virtual machine, originally
 * written in EMCA Script.
 *
 * The VM consists of a single operation:
 *	add *src, inc, *dst
 *
 * The add operation takes a source pointer, adds the given number (inc) and
 * stores the result to the memory dst.
 *
 * Registers keep track of various state-related numbers, such as accumulators
 * and decumulators, as well as maintaining stack pointers.
 */

#include <malloc.h>
#include <memory.h>

#include "gleam_emu.h"
#include "gleam_mem.h"

gleam_emu_t *emu_new (void) {
	gleam_emu_t *emu = (gleam_emu_t*)malloc(sizeof(gleam_emu_t));

		emu->mem = mem_new();
		if (!emu->mem) {
			DBG("No mem!\n");
			free(emu);
			return 0;
		}

		if (mem_test(emu->mem)) {
			DBG("Mem test failed!\n");
			mem_free(emu->mem);
			free(emu);
			return 0;
		}
	return emu;
}

gnum emu_test (gleam_emu_t *emu) {
	gnum ret;
	
	ret = mem_test(emu->mem);
	mem_dump(emu->mem);

	return ret;
}

void emu_free (gleam_emu_t *emu) {
	mem_free(emu->mem);
	free(emu);
}

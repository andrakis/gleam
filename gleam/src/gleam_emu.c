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

#include <malloc.h>		// malloc, free
#include <stdlib.h>		// atoll

#include "platform.h"
#include "gleam_debug.h"
#include "gleam_emu.h"
#include "gleam_mem.h"

void emu_rom (gleam_emu_t *emu);

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
		
		emu_rom(emu);
	return emu;
}

/**
 * Write the boot code and other various things into the CPU memory.
 */
void emu_rom (gleam_emu_t *emu) {
	gleam_mem_t *mem = emu->mem;

	// Write 0 to location 0
	// Write a small code fragment that sets cp to 0
	mem_write(0, 0, mem);
	mem_write(1, 0, mem);
	mem_write(2, cp, mem);
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

char *emu_decode_pos (gnum pos) {
	switch(pos) {
		case a0: return "a0";
		case cp: return "cp";
		case rp: return "rp";
		case sp: return "sp";
		case psp: return "psp";
		case eq: return "eq";
		case mt: return "mt";
		case lt: return "lt";
		case ac: return "ac";
		case dc: return "dc";
		case r0: return "r0";
		case r1: return "r1";
		case r2: return "r2";
		case r3: return "r3";
		case r4: return "r4";
	}

	return 0;
}

gnum emu_encode_pos (char *symbol) {
	if (*symbol == 'a' && *(symbol + 1) == '0') return a0;
	if (*symbol == 'c' && *(symbol + 1) == 'p') return cp;
	if (*symbol == 'r' && *(symbol + 1) == 'p') return rp;
	if (*symbol == 's' && *(symbol + 1) == 'p') return sp;
	if (*symbol == 'p' && *(symbol + 1) == 's' && *(symbol + 2) == 'p')
			return psp;
	if (*symbol == 'e' && *(symbol + 1) == 'q') return eq;
	if (*symbol == 'm' && *(symbol + 1) == 't') return mt;
	if (*symbol == 'l' && *(symbol + 1) == 't') return lt;
	if (*symbol == 'a' && *(symbol + 1) == 'c') return ac;
	if (*symbol == 'd' && *(symbol + 1) == 'c') return dc;
	if (*symbol == 'r' && *(symbol + 1) == '0') return r0;
	if (*symbol == 'r' && *(symbol + 1) == '1') return r1;
	if (*symbol == 'r' && *(symbol + 1) == '2') return r2;
	if (*symbol == 'r' && *(symbol + 1) == '3') return r3;
	if (*symbol == 'r' && *(symbol + 1) == '3') return r2;

	return atoll(symbol);
}

/**
 * Execute an iteration
 */
void emu_iterate (gleam_emu_t *emu) {
	register gleam_mem_t *mem = emu->mem;
	gnum _sp,	// Stack pointer
	     from,	// From pointer
	     add,	// How much to add
	     to,	// Destination
	     val;	// The final value
	
	// Read stack pointer
	// var _sp = self._memory[self._memory[sp] + sp];
	_sp = mem_read(mem_read(sp, mem) + sp, mem);
	// Read from code pointer, and increment code pointer
	// The || 0 is now implied.
	// var from = self._memory[_sp + self._memory[_sp + cp]++] || 0;
	from = mem_read(_sp + mem_read(_sp + cp, mem), mem);
	mem_write(_sp + cp, mem_read(_sp + cp, mem) + 1, mem);
	// Read number to add, increment cp
	// var add = self._memory[_sp + self._memory[_sp + cp]++];
	add = mem_read(_sp + mem_read(_sp + cp, mem), mem);
	mem_write(_sp + cp, mem_read(_sp + cp, mem) + 1, mem);
	// Read destination, increment cp
	// var to = self._memory[_sp + self._memory[_sp + cp]++];
	to = mem_read(_sp + mem_read(_sp + cp, mem), mem);
	mem_write(_sp + cp, mem_read(_sp + cp, mem) + 1, mem);
	// Assign. The || 0 is now implied.
	// var v = self._memory[_sp + to] = ((self._memory[_sp + from] || 0) + add);
	val = mem_read(_sp + from, mem) + add;
	mem_write(_sp + to, val, mem);

	// Debugging
	GLEAM_DBG(from, add, to, val);

	// Compute the various other register values
	mem_write(_sp + eq, (val == 0 ? 3 : 0), mem);
	mem_write(_sp + mt, (val  > 0 ? 3 : 0), mem);
	mem_write(_sp + lt, (val  < 0 ? 3 : 0), mem);
	// Accumulate
	// self._memory[_sp + ac] += v;
	mem_write(_sp + ac, mem_read(_sp + ac, mem) + val, mem);
	// Decumulate
	mem_write(_sp + dc, mem_read(_sp + dc, mem) - val, mem);
}

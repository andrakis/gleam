/**
 * Gleam emulator header
 */

#ifndef _GLEAM_EMU_H
#define _GLEAM_EMU_H

#include "gleam.h"

struct gleam_regs {
	// Required registers
	gnum   a0,		// Absolute 0 (offset required to access 0)
	       cp,		// Code pointer
		   rp,		// Return pointer
		   sp,		// Stack pointer
		   psp,		// Previous stack pointer
	// Operational registers - these are recalculated at each iteration
		   eq,		// New value == 0
		   mt,		// New value > 0
		   lt,		// New value < 0
		   ac,		// Accumulator
		   dc,		// Decumulator
	// General purpose registers
		   r0,
		   r1,
		   r2,
		   r3,
		   r4;
};

#endif

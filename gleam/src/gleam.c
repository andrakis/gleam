// gleam.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

#include "platform.h"
#include "gleam_emu.h"
#include "gleam_mem.h"

int main(int argc, char* argv[])
{
	gleam_emu_t *emu = emu_new();
	if (!emu) {
		return 1;
	}

	emu_test(emu);

	emu_free(emu);
	return 0;
}


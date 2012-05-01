#include <stdio.h>

#include "gleam.h"
#include "gleam_debug.h"
#include "gleam_emu.h"

void print_part (const char *prefix, gnum pos) {
	char *decoded = emu_decode_pos(pos);

	printf("%s", prefix);
	if (decoded) {
		printf("%s", decoded);
	} else {
		printf("[%lld]", pos);
	}
}

void gleam_debug(gnum from, gnum add, gnum to, gnum val)
{
	print_part("", from);
	print_part(" + ", add);
	print_part(" -> ", to);
	printf(" (%lld)\n", val);
}


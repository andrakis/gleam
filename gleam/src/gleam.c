// gleam.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

#include "platform.h"
#include "gleam_mem.h"

int main(int argc, char* argv[])
{
	gnum mem;

	if (mem_init()) {
		printf("Mem init error!\n");
		getch();
		return 1;
	}

	mem = mem_test();
	if (mem) {
		printf("Mem test error! %ld\n", mem);
		getch();
		return 2;
	}

	printf("init ok!\n");

	mem_dump();
	getch();
	return 0;
}


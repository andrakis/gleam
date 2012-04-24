CC=gcc
CFLAGS=-Wall -g -O2 -Iinclude
OBJECTS=gleam.o gleam_emu.o gleam_mem.o
OUTPUT=gleam

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTPUT)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $<

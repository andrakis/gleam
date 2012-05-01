CC=gcc
CFLAGS=-Wall -g -O2 -Iinclude
SRC_DIR=src
BUILD_DIR=.
SOURCES=$(wildcard $(SRC_DIR)/*.c)
TARGETS=$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
OUTPUT=gleam

$(OUTPUT): $(TARGETS)
	$(CC) $(CFLAGS) $(TARGETS) -o $(OUTPUT)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

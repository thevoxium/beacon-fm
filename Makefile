CC      := cc
BUILD   := build
SRC     := src/main.c src/fm.c
TARGET  := $(BUILD)/main

CFLAGS  := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wformat=2 -Wnull-dereference -Wstrict-prototypes -Werror -Isrc
LDFLAGS := -lncurses

.PHONY: run clean

build: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

run: build
	./$(TARGET)

clean:
	rm -rf $(BUILD)/*

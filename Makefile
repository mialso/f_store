CFLAGS := -g -Wall -Wextra -Wpedantic -std=c99 -fPIC -Iinclude/
SHFLAGS := -shared
CC := gcc

BIN_DIR := $(shell pwd)/bin/
GWAN_PATH := ~/projects/vs_pm/gwan_linux64-bit/
lib_name := libfstore.so

objects := f_store.o test_fs.o
test_target := $(BIN_DIR)run_tests

all: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(test_target)
	#@rm *.o
	@echo "tests running..."
	bash -c $(test_target)


f_store.o: src/f_store.c include/f_store.h
	$(CC) $(CFLAGS) -c src/f_store.c

test_fs.o: test/test_fs.c include/f_store.h
	$(CC) $(CFLAGS) -c test/test_fs.c

lib: f_store.o
	$(CC) $(SHFLAGS) $^ -o $(BIN_DIR)$(lib_name)

install:
	cp $(BIN_DIR)$(lib_name) $(GWAN_PATH)libraries/
	cp $(shell pwd)/include/f_store.h $(GWAN_PATH)include/

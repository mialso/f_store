CFLAGS := -g -Wall -Wextra -Wpedantic -std=c99 -Iinclude/
CC := gcc

BIN_DIR := $(shell pwd)/bin/

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

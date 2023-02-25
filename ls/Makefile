CC=cc
CFLAGS=-I. -Wall
BUILD_DIR=build

ls: ls.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/ls $(BUILD_DIR)/ls.o

ls.o : ls.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -g -c ls.c -o $(BUILD_DIR)/ls.o

$(BUILD_DIR):
	mkdir -p build
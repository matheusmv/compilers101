BIN=ast

SOURCE_FILES=$(wildcard *.c)

CC=gcc

CFLAGS=-pedantic -Wall -Werror -std=c11
LFLAGS=

VALGRIND=valgrind -s \
		 --tool=memcheck \
		 --leak-check=full \
		 --leak-resolution=med \
		 --track-origins=yes \
		 --show-leak-kinds=all \
		 --vgdb=no

all: CFLAGS += -O2
all: $(BIN)

run: all
run:
	./$(BIN)

debug: CFLAGS += -O0 -g
debug: $(BIN)

valgrind: debug
valgrind:
	$(VALGRIND) ./$(BIN)

$(BIN): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

clean:
	$(RM) $(BIN)

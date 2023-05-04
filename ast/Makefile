BIN=ast
TEST_BIN=test

MAIN_FILE=main.c
TEST_FILE=tests.c

SOURCE_FILES=$(wildcard src/*.c)
TEST_FILES=$(wildcard tests/**/*.c)

CC=gcc

CFLAGS=-Wall -Wextra -Werror -std=c11
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
	./$(BIN)

debug: CFLAGS += -O0 -g
debug: $(BIN)

valgrind: debug
valgrind:
	$(VALGRIND) ./$(BIN)

test: CFLAGS += -g -O0
test: SOURCE_FILES += tests.c
test: $(TEST_FILES) $(SOURCE_FILES) $(TEST_FILE)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

run_tests: test
run_tests:
	$(VALGRIND) ./$(TEST_BIN)

$(BIN): $(SOURCE_FILES) $(MAIN_FILE)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

clean:
	$(RM) $(BIN) $(TEST_BIN)

BIN = ast
TEST_BIN = test
PARSER_BIN = rose

MAIN_FILE = main.c
TEST_FILE = tests.c

SOURCE_FILES = $(wildcard src/*.c)

TEST_FILES = $(wildcard tests/**/*.c)

CC = gcc

CFLAGS = -Wall -Wextra

LFLAGS = -lm

VALGRIND = valgrind -s \
           --tool=memcheck \
           --leak-check=full \
           --leak-resolution=med \
           --track-origins=yes \
           --show-leak-kinds=all \
           --vgdb=no


$(BIN): $(SOURCE_FILES) $(MAIN_FILE)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)


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


rose: parser.c rose.tab.c rose.lex.c $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)


debug_rose: CFLAGS += -O0 -g
debug_rose: rose
debug_rose:
	$(VALGRIND) ./$(PARSER_BIN) example.rose


rose.tab.c: rose.y
	bison -d -o $@ $<

rose.lex.c: rose.l
	flex -o $@ $<


clean:
	$(RM) $(BIN) $(TEST_BIN) rose rose.tab.* rose.lex.* rose.output

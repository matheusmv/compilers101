BIN=calculator
LEX_SRC=lex.yy.cpp
BISON_SRC=parser.tab.cpp

SOURCE_FILES=$(wildcard *.cpp)

CC=g++
FLEX=flex
BISON=bison

CFLAGS=-Wall -std=c++17
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
	./$(BIN) example.txt

debug: CFLAGS += -O0 -g
debug: FLEX += -d -v
debug: BISON += -v
debug: $(BIN)

valgrind: debug
valgrind:
	$(VALGRIND) ./$(BIN) example.txt

$(BIN): $(LEX_SRC) $(BISON_SRC) $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

$(LEX_SRC): *.l
	$(FLEX) -o $@ -i $^

$(BISON_SRC): *.y
	$(BISON) -d $^ -o $@

clean:
	$(RM) $(BIN) $(LEX_SRC) parser.tab.* *.output location.*

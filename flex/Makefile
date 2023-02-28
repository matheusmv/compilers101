BIN=echo
LEX_SRC=lex.yy.cc

CC=g++
LEX=flex++

CFLAGS=-Wall -std=c++17

VALGRIND=valgrind -s \
		 --tool=memcheck \
		 --leak-check=full \
		 --leak-resolution=med \
		 --track-origins=yes \
		 --vgdb=no

all: CFLAGS += -O2
all: $(BIN)

debug: CFLAGS += -O0 -g
debug: LEX += -d -v
debug: $(BIN)

valgrind: debug
valgrind:
	$(VALGRIND) ./$(BIN)

$(BIN): $(LEX_SRC) *.cpp
	$(CC) $(CFLAGS) $^ -o $@

$(LEX_SRC): *.l
	$(LEX) $^

clean:
	$(RM) $(BIN) $(LEX_SRC)

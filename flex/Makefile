BIN=echo
LEX_SRC=lex.yy.c

CC=gcc
LEX=flex

CFLAGS=-Wall -std=c11

all: $(BIN)

$(BIN): $(LEX_SRC) *.c
	$(CC) $(CFLAGS) $^ -o $@

$(LEX_SRC): *.l
	$(LEX) $^

clean:
	$(RM) $(BIN) $(LEX_SRC)
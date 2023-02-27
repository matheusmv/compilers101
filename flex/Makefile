BIN=echo
LEX_SRC=lex.yy.cc

CC=g++
LEX=flex++

CFLAGS=-Wall -std=c++17

all: $(BIN)

$(BIN): $(LEX_SRC) *.cpp
	$(CC) $(CFLAGS) $^ -o $@

$(LEX_SRC): echo.l
	$(LEX) $^

clean:
	$(RM) $(BIN) $(LEX_SRC)
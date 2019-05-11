CC ?= gcc
UNAME := $(shell uname -s)
BIN = rotwkl


SRC = $(wildcard src/*.c)
OBJ := $(addsuffix .o, $(basename $(SRC)))

INC = -I src/

export CPFLAGS

CFLAGS := $(CFLAGS) -std=c11 -Wall -Wextra -pedantic $(INC)
LIBS = -lssl -lcrypto

ifeq ($(OS), Windows_NT)
    LIBS += -L "%PROGRAMFILES(x86)\OpenSSL-win32\lib\VC"
endif

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean run

clean:
	rm -f $(OBJ) $(BIN)

run: $(BIN)
	./$(BIN)

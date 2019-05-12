CC ?= gcc
UNAME := $(shell uname -s)
BIN = rotwkl

SRC = $(wildcard src/*.c)
RC = $(wildcard src/*.rc)
OBJ := $(addsuffix .o, $(basename $(SRC)))

INC = -I src/ 

ifeq ($(OS), Windows_NT)
    INC += -I "C:\msys64\mingw64\include"
    LFLAGS += -L "C:\msys64\mingw64\lib"
endif

CFLAGS := $(CFLAGS) -std=c11 -Wall -Wextra -pedantic -fopenmp $(INC)
LIBS = $(LFLAGS) -lssl -lcrypto

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 

.PHONY: clean run

clean:
	rm -f $(OBJ) $(BIN)

run: $(BIN)
	./$(BIN)

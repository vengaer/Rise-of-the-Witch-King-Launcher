CC ?= gcc
UNAME := $(shell uname -s)
BIN = rotwkl

SRC = $(wildcard src/*.c)
RC = $(wildcard src/*.rc)
OBJ := $(addsuffix .o, $(basename $(SRC)))

INC = -I src/ 

CFLAGS := $(CFLAGS) -std=c11 -Wall -Wextra -pedantic -fopenmp `pkg-config --cflags gtk+-3.0` $(INC)
LDFLAGS = -static-libgcc -lssl -lcrypto -lgomp -lpthread `pkg-config --libs gtk+-3.0`

ifeq ($(OS), Windows_NT)
    INC += -I "C:/msys64/mingw64/include"
    LDFLAGS := -L "C:/msys64/mingw64/lib" -static $(LDFLAGS)
endif

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) 

.PHONY: clean run release

clean:
	rm -f $(OBJ) $(BIN)

run: $(BIN)
	./$(BIN)

release: CFLAGS += -o3
release: $(BIN)

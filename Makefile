CC ?= gcc
UNAME := $(shell uname -s)
BIN = rotwkl

SRC = $(wildcard src/*.c)
RC = $(wildcard src/*.rc)
OBJ := $(addsuffix .o, $(basename $(SRC)))

INC = -I src/ 

CFLAGS := $(CFLAGS) -std=c11 -Wall -Wextra -pedantic -fopenmp `pkg-config --cflags gtk+-3.0` $(INC)
LDFLAGS = -static-libgcc

ifeq ($(OS), Windows_NT)
    INC += -I "C:/msys64/mingw64/include"
    LDFLAGS := -L "C:/msys64/mingw64/lib" $(LDFLAGS) -l:libssl.a -l:libcrypto.a -l:libgomp.a -l:libpthread.a -lgtk-3 -lgdk-3 -l:libgdi32.a -l:libimm32.a -l:libshell32.a -l:libole32.a -l:libuuid.a -l:libwinmm.a -l:libdwmapi.a -l:libsetupapi.a -lcfgmgr32 -l:libz.a -l:libpangowin32-1.0.a -l:libpangocairo-1.0.a -l:libpango-1.0.a -latk-1.0 -l:libcairo-gobject.a -l:libcairo.a -lgdk_pixbuf-2.0 -l:libgio-2.0.a -lgobject-2.0 -l:libintl.a -lglib-2.0
else
    LDFLAGS := $(LDFLAGS) -static-libgcc -lssl -lcrypto -lgomp -lpthread `pkg-config --libs gtk+-3.0`
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

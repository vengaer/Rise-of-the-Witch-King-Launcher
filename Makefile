CC ?= gcc
CXX ?= g++

BIN := rotwkl
SRC_DIR := src
LIB_DIR := lib
LIB := $(LIB_DIR)/librotwk.a

SRC := $(wildcard $(SRC_DIR)/*.c)
CXXSRC :=$(wildcard $(SRC_DIR)/*.cc)
OBJ := $(addsuffix .o, $(basename $(SRC)))

INC = -I src/ 
OBJ_DIR = obj

CFLAGS := $(CFLAGS) -c -std=c11 -Wall -Wextra -pedantic -fopenmp `pkg-config --cflags gtk+-3.0` $(INC)
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fopenmp $(INC)
LDFLAGS = -static-libgcc -l:libssl.a -l:libcrypto.a -l:libgomp.a -l:libpthread.a -lgtk-3 -lgdk-3 -l:libgdi32.a -l:libimm32.a -l:libshell32.a -l:libole32.a -l:libuuid.a -l:libwinmm.a -l:libdwmapi.a -l:libsetupapi.a -lcfgmgr32 -l:libz.a -l:libpangowin32-1.0.a -l:libpangocairo-1.0.a -l:libpango-1.0.a -latk-1.0 -l:libcairo-gobject.a -l:libcairo.a -lgdk_pixbuf-2.0 -l:libgio-2.0.a -lgobject-2.0 -l:libintl.a -lglib-2.0

ifeq ($(OS), Windows_NT)
    INC += -I "C:/msys64/mingw64/include"
    LDFLAGS := -L "C:/msys64/mingw64/lib" $(LDFLAGS)
endif

$(BIN): LDFLAGS := -L lib/ -l:librotwk.a $(LDFLAGS)
$(BIN): $(LIB)
	$(CXX) $(CXXFLAGS) $(CXXSRC) $(LDFLAGS)  -o $@

$(LIB): $(OBJ) setup
	ar rcs $(LIB) $(OBJ)

.PHONY: clean run release setup 

clean:
	rm -f $(OBJ) $(BIN) $(LIB)

run: $(BIN)
	./$(BIN)

release: CFLAGS += -o3
release: $(BIN)

setup:
	mkdir -p $(LIB_DIR)


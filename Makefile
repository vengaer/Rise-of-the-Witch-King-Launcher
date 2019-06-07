CC ?= gcc
CXX ?= g++

BIN := rotwkl
SRC_DIR := src
LIB_DIR := lib
LIB := $(LIB_DIR)/librotwk.a
GTK_DIR := $(SRC_DIR)/gtk

SRC := $(wildcard $(SRC_DIR)/*.c)
CXXSRC :=$(wildcard $(SRC_DIR)/*.cc)

OBJ := $(addsuffix .o, $(basename $(SRC)))
CXXOBJ := $(addsuffix .o, $(basename $(CXXSRC)))

GTK_SRC := $(wildcard $(GTK_DIR)/*.c)
GTK_OBJ := $(addsuffix .o, $(basename $(GTK_SRC)))

XML_SRC := $(wildcard xml/*.ui)
XML_HEADER := $(SRC_DIR)/ui_mainwindow.h

QT_HEADERS := $(SRC_DIR)/mainwindow.h
MOC_HEADERS := $(addsuffix .moc.cc, $(basename $(QT_HEADERS)))

QT_VER := 5.12.3
QT_PATH := D:/Qt/$(QT_VER)/mingw73_64
UIC := $(QT_PATH)/bin/uic.exe

MOC := $(QT_PATH)/bin/moc.exe

INC = -I src/ 
OBJ_DIR = obj

CFLAGS := $(CFLAGS) -c -std=c11 -Wall -Wextra -pedantic -fopenmp $(INC)
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fopenmp $(INC)
LDFLAGS = -static-libgcc -static-libstdc++ -lssl -lcrypto -lgomp -lpthread

ifeq ($(OS), Windows_NT)
    INC += -I "C:/msys64/mingw64/include"
    LDFLAGS := -L "C:/msys64/mingw64/lib" -static $(LDFLAGS) 
endif

.PHONY: clean run release setup gtk_build

$(BIN): INC += -I $(QT_PATH)/include -I $(QT_PATH)/include/QtWidgets -I $(QT_PATH)/include/QtGui -I $(QT_PATH)/include/QtCore
$(BIN): CXXFLAGS += $(INC)
$(BIN): LDFLAGS := -L lib/ -l:librotwk.a -L $(QT_PATH)/lib -lQt5Widgets -lQt5Gui -lQt5Core -lqtmain $(LDFLAGS)
$(BIN): $(LIB) $(XML_HEADER) $(MOC_HEADERS) $(CXXOBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(CXXOBJ) $(LDFLAGS)

$(XML_HEADER):
	$(UIC) -o $@

$(MOC_HEADERS): $(QT_HEADERS)
	$(MOC) $(INC) $< -o $@

gtk_build: LDFLAGS := -L lib/ -lrotwk $(LDFLAGS)
gtk_build: CXXFLAGS += -D GTK_GUI -I $(GTK_DIR)
gtk_build: OBJ += $(GTK_OBJ)
gtk_build: LDFLAGS += `pkg-config --libs gtk+-3.0`
gtk_build: $(GTK_OBJ) $(LIB)
	$(CXX) $(CXXFLAGS) $(CXXSRC) $(LDFLAGS)  -o $@

$(LIB): CFLAGS += -o3
$(LIB): $(OBJ) setup
	ar rcs $(LIB) $(OBJ)

$(GTK_OBJ): CFLAGS += `pkg-config --cflags gtk+-3.0`
$GTK_OBJ): $(GTK_SRC)
	$(CC) -c $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN) $(LIB, $(GTK_OBJ)); rm -rf $(LIB_DIR)

run: $(BIN)
	./$(BIN)

release: CFLAGS += -o3
release: $(BIN)

setup:
	mkdir -p $(LIB_DIR)

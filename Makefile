CC ?= gcc
CXX ?= g++

BIN := rotwkl
SRC_DIR := src
LIB_DIR := lib
LIB := $(LIB_DIR)/librotwk.a

SRC := $(wildcard $(SRC_DIR)/*.c)
CXXSRC :=$(wildcard $(SRC_DIR)/*.cc)

OBJ := $(addsuffix .o, $(basename $(SRC)))
CXXOBJ := $(addsuffix .o, $(basename $(CXXSRC))) 

QT_HEADERS := $(SRC_DIR)/mainwindow.h
MOC_DIR := $(SRC_DIR)/moc
MOC_HEADERS := $(MOC_DIR)/$(addsuffix .moc.cc, $(basename $(notdir $(QT_HEADERS))))
MOC_OBJ := $(addsuffix .o, $(basename $(MOC_HEADERS)))

XML_SRC := $(wildcard xml/*.ui)
XML_HEADER := $(SRC_DIR)/ui_mainwindow.h

QT_VER := 5.12.3
QT_PATH := D:/Qt/$(QT_VER)/mingw73_64
UIC := $(QT_PATH)/bin/uic.exe

MOC := $(QT_PATH)/bin/moc.exe

INC = -I src/ 

CFLAGS := $(CFLAGS) -c -std=c11 -O3 -Wall -Wextra -pedantic -fopenmp $(INC)
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fopenmp $(INC)
LDFLAGS = -static-libgcc -static-libstdc++ -lssl -lcrypto -lgomp -lstdc++ -lpthread 

ifeq ($(OS), Windows_NT)
    INC += -I "C:/msys64/mingw64/include"
    LDFLAGS := -L "C:/msys64/mingw64/lib" -static $(LDFLAGS) 
endif

.PHONY: clean run release setup 

$(BIN): INC += -I $(QT_PATH)/include -I $(QT_PATH)/include/QtWidgets -I $(QT_PATH)/include/QtGui -I $(QT_PATH)/include/QtCore
$(BIN): CXXFLAGS += $(INC)
$(BIN): LDFLAGS := -L lib/ -l:librotwk.a -L $(QT_PATH)/lib -lQt5Widgets -lQt5Gui -lQt5Core -lqtmain $(LDFLAGS)
$(BIN): setup $(XML_HEADER) $(MOC_OBJ) $(LIB) $(CXXOBJ) 
	$(CXX) -o $@ $(CXXFLAGS) $(CXXOBJ) $(MOC_OBJ) $(LDFLAGS)

$(XML_HEADER):
	$(UIC) -o $@

$(MOC_OBJ): $(MOC_HEADERS)
	$(CXX) -c -o $@ $< $(INC) $(LDFLAGS)

$(MOC_HEADERS): $(QT_HEADERS)
	$(MOC) $(INC) $< -o $@

$(LIB): $(OBJ) setup
	ar rcs $(LIB) $(OBJ)

release: CXXFLAGS += -O3
release: $(BIN)

clean:
	rm -f $(OBJ) $(BIN) $(LIB) $(MOC_HEADERS) $(MOC_OBJ) $(CXXOBJ); rm -rf $(LIB_DIR) $(MOC_DIR)

run: $(BIN)
	./$(BIN)

setup:
	mkdir -p $(LIB_DIR) $(MOC_DIR)

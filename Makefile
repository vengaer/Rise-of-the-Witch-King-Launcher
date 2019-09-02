CC ?= gcc
CXX ?= g++

BIN := rotwkl
SRC_DIR := src
LIB_DIR := lib
LIB := $(LIB_DIR)/librotwk.a

REGSRC := $(SRC_DIR)/pattern.cc
SRC := $(wildcard $(SRC_DIR)/*.c)
CXXSRC :=$(filter-out $(REGSRC), $(wildcard $(SRC_DIR)/*.cc))

REGOBJ := $(addsuffix .o, $(basename $(REGSRC)))
OBJ := $(addsuffix .o, $(basename $(SRC)))
CXXOBJ := $(addsuffix .o, $(basename $(CXXSRC)))

QT_HEADERS := $(SRC_DIR)/mainwindow.h
MOC_DIR := $(SRC_DIR)/moc
MOC_HEADERS := $(MOC_DIR)/$(addsuffix .moc.cc, $(basename $(notdir $(QT_HEADERS))))
MOC_OBJ := $(addsuffix .o, $(basename $(MOC_HEADERS)))

XML_SRC := $(wildcard xml/*.ui)
XML_HEADER := $(SRC_DIR)/ui_mainwindow.h

INC = -I src/

QT_LDFLAGS := -lQt5Widgets -lQt5Gui -lQt5Core

ifeq ($(OS), Windows_NT)
	QT_VER := 5.12.3
	QT_PATH := D:/Qt/$(QT_VER)/mingw73_64

	UIC := $(QT_PATH)/bin/uic.exe
	MOC := $(QT_PATH)/bin/moc.exe

    INC += -I C:/msys64/mingw64/include
    LDFLAGS := -L C:/msys64/mingw64/lib -L C:/msys64/usr/local/ssl -static $(LDFLAGS)

	QT_INCLUDE := -I $(QT_PATH)/include -I $(QT_PATH)/include/QtWidgets -I $(QT_PATH)/include/QtGui -I $(QT_PATH)/include/QtCore
	QT_FLAGS := $(QT_INCLUDE)
	QT_LDFLAGS := -L $(QT_PATH)/lib $(QT_LDFLAGS) -lqtmain
else
	QT_PATH := /usr/include/qt
	UIC := uic
	MOC := moc
	QT_INCLUDE := -I $(QT_PATH) -I $(QT_PATH)/QtWidgets -I $(QT_PATH)/QtGui -I $(QT_PATH)/QtCore
	QT_FLAGS := -fPIC $(QT_INCLUDE)
endif


CFLAGS := $(CFLAGS) -c -std=c11 -O3 -Wall -Wextra -pedantic -Wunknown-pragmas -fopenmp $(INC)
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Wunknown-pragmas -fopenmp $(INC)
LDFLAGS = -static-libgcc -static-libstdc++ -lssl -lcrypto -lgomp -lstdc++ -lpthread -L lib/

ifeq ($(OS), Windows_NT)
	LDFLAGS += -l:libinput.a
else
	LDFLAGS += -linput
endif

.PHONY: clean run release setup lib

$(BIN): CXXFLAGS += $(QT_FLAGS)
$(BIN): LDFLAGS := -L lib/ -l:librotwk.a $(QT_LDFLAGS) $(LDFLAGS)
$(BIN): setup $(XML_HEADER) $(MOC_OBJ) $(LIB) $(CXXOBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(CXXOBJ) $(MOC_OBJ) $(LDFLAGS)

$(XML_HEADER):
	$(UIC) -o $@

$(MOC_OBJ): $(MOC_HEADERS)
	$(CXX) -c -o $@ $< $(QT_FLAGS)

$(MOC_HEADERS): $(QT_HEADERS)
	$(MOC) $(INC) $< -o $@

$(LIB): $(REGOBJ) $(OBJ) setup
	ar rcs $(LIB) $(REGOBJ) $(OBJ)

release: CXXFLAGS += -O3
release: $(BIN)

lib: $(LIB)

clean:
	rm -f $(OBJ) $(BIN) $(LIB) $(MOC_HEADERS) $(MOC_OBJ) $(CXXOBJ) $(REGOBJ); rm -rf $(LIB_DIR) $(MOC_DIR)

run: $(BIN)
	./$(BIN)

setup:
	mkdir -p $(LIB_DIR) $(MOC_DIR)

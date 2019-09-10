CC ?= gcc
CXX ?= g++

BIN := rotwkl
SRC_DIR := src

CSRC := $(wildcard $(SRC_DIR)/*.c)
CXXSRC := $(wildcard $(SRC_DIR)/*.cc)

COBJ := $(addsuffix .o, $(basename $(CSRC)))
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
	QT_PATH := D:/Qt5.13/5.13.1/mingw73_64

	UIC := $(QT_PATH)/bin/uic.exe
	MOC := $(QT_PATH)/bin/moc.exe

    INC += -I C:/msys64/mingw64/include
    LDFLAGS := -L C:/msys64/mingw64/lib -static $(LDFLAGS)

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


CFLAGS := $(CFLAGS) -c -std=c11 -Wall -Wextra -pedantic -Wunknown-pragmas -fopenmp $(INC)
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Wunknown-pragmas -fopenmp $(INC)
LDFLAGS := $(LDFLAGS) -static-libgcc -static-libstdc++ -lssl -lcrypto -lgomp -lstdc++ -lpthread -L lib/

ifeq ($(OS), Windows_NT)
	LDFLAGS += -l:libinput.a
else
	LDFLAGS += -linput
endif

.PHONY: clean run release setup

$(BIN): CXXFLAGS += $(QT_FLAGS)
$(BIN): LDFLAGS := $(QT_LDFLAGS) $(LDFLAGS)
$(BIN): setup $(XML_HEADER) $(MOC_OBJ) $(COBJ) $(CXXOBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(MOC_OBJ) $(COBJ) $(CXXOBJ) $(LDFLAGS)

$(XML_HEADER):
	$(UIC) -o $@

$(MOC_OBJ): $(MOC_HEADERS)
	$(CXX) -c -o $@ $< $(QT_FLAGS)

$(MOC_HEADERS): $(QT_HEADERS)
	$(MOC) $(INC) $< -o $@

release: CFLAGS += -O3
release: CXXFLAGS += -O3
release: $(BIN)

clean:
	rm -f $(COBJ) $(BIN) $(MOC_HEADERS) $(MOC_OBJ) $(CXXOBJ); rm -rf $(MOC_DIR)

run: $(BIN)
	./$(BIN)

setup:
	mkdir -p $(MOC_DIR)

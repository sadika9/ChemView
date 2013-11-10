QT       -= gui

QMAKE_CXXFLAGS += -std=c++11

TARGET = ChemView
TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    molecule.h \
    atom.h \
    bond.h \
    cmlreader.h

SOURCES += \
    molecule.cpp \
    atom.cpp \
    bond.cpp \
    cmlreader.cpp

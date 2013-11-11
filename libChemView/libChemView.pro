QT       -= gui
QT       += opengl

QMAKE_CXXFLAGS += -std=c++11

TARGET = ChemView
TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    molecule.h \
    atom.h \
    bond.h \
    cmlreader.h \
    openglwidget.h \
    cubegeometry.h \
    geometryengine.h

SOURCES += \
    molecule.cpp \
    atom.cpp \
    bond.cpp \
    cmlreader.cpp \
    openglwidget.cpp \
    cubegeometry.cpp \
    geometryengine.cpp

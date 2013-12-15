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
    mesh.h \
    obreader.h \
    camera.h

SOURCES += \
    molecule.cpp \
    atom.cpp \
    bond.cpp \
    cmlreader.cpp \
    openglwidget.cpp \
    mesh.cpp \
    obreader.cpp \
    camera.cpp

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += openbabel-2.0

RESOURCES += \
    meshes.qrc \
    shaders.qrc

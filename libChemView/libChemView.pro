QT       -= gui
QT       += opengl

DEFINES -= "USE_ASSIMP"

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
    obreader.h

SOURCES += \
    molecule.cpp \
    atom.cpp \
    bond.cpp \
    cmlreader.cpp \
    openglwidget.cpp \
    mesh.cpp \
    obreader.cpp

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += assimp
unix: PKGCONFIG += openbabel-2.0

RESOURCES += \
    meshes.qrc \
    shaders.qrc

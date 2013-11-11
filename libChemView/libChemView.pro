QT       -= gui
QT       += opengl

DEFINES += "USE_ASSIMP"

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
    geometryengine.h \
    mesh.h

SOURCES += \
    molecule.cpp \
    atom.cpp \
    bond.cpp \
    cmlreader.cpp \
    openglwidget.cpp \
    cubegeometry.cpp \
    geometryengine.cpp \
    mesh.cpp

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += assimp

RESOURCES += \
    meshes.qrc

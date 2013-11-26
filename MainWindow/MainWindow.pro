#-------------------------------------------------
#
# Project created by QtCreator 2013-11-11T21:51:36
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

TARGET = ChemViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libChemView/release/ -lChemView
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libChemView/debug/ -lChemView
else:unix: LIBS += -L$$OUT_PWD/../libChemView/ -lChemView

INCLUDEPATH += $$PWD/../libChemView
DEPENDPATH += $$PWD/../libChemView

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libChemView/release/ChemView.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libChemView/debug/ChemView.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libChemView/libChemView.a

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += assimp
unix: PKGCONFIG += openbabel-2.0

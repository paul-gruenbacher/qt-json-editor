# -------------------------------------------------
# Project created by QtCreator 2009-11-07T11:39:43
# -------------------------------------------------
TARGET = qtfindreplacedialog
TEMPLATE = lib
CONFIG += static
DEFINES += FINDREPLACE_LIBRARY
SOURCES += findform.cpp \
    finddialog.cpp \
    findreplaceform.cpp \
    findreplacedialog.cpp
HEADERS += findreplaceform.h \
    findreplacedialog.h \
    findform.h \
    finddialog.h \
    findreplace_global.h
FORMS += findreplaceform.ui \
    findreplacedialog.ui
DESTDIR = ../libs
DEPENDPATH += .
target.path = /libs
headers.files = $$HEADERS
headers.path = /include
INSTALLS += target \
    headers
QT += core gui widgets

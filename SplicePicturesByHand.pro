#-------------------------------------------------
#
# Project created by QtCreator 2015-10-20T14:24:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SplicePicturesByHand
TEMPLATE = app


SOURCES += main.cpp\
        splicepicturesbyhand.cpp \
    splicepicturespanel.cpp \
    rational.cpp \
    splicepicturesimageitem.cpp \
    splicepicturescalibrationitem.cpp \
    splicepicturesbackgrounditem.cpp \
    backgrounddodging.cpp \
    autostitch.cpp \
    pir.c

HEADERS  += splicepicturesbyhand.h \
    splicepicturespanel.h \
    rational.h \
    splicepicturesimageitem.h \
    splicepicturescalibrationitem.h \
    splicepicturesbackgrounditem.h \
    backgrounddodging.h \
    autostitch.h \
    pir.h

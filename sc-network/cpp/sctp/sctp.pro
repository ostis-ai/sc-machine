#-------------------------------------------------
#
# Project created by QtCreator 2012-10-19T11:26:31
#
#-------------------------------------------------

QT       -= gui

TARGET = sctp
TEMPLATE = lib

DEFINES += SCTP_LIBRARY

SOURCES += sctp.cpp

HEADERS += sctp.h\
        sctp_global.h

CONFIG (debug, debug|release) {
    DESTDIR = ../bin
} else {
    DESTDIR = ../bin
}

OBJECTS_DIR = obj
MOC_DIR = moc

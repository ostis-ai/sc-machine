#-------------------------------------------------
#
# Project created by QtCreator 2012-10-19T11:26:31
#
#-------------------------------------------------

QT       -= gui

TARGET = sctp
TEMPLATE = lib
VERSION = 0.1.0

DEFINES += SCTP_LIBRARY

SOURCES += sctp.cpp \
           sctpCommand.cpp

HEADERS += sctp.h\
        sctp_global.h \
        sctpCommand.h

CONFIG (debug, debug|release) {
    DESTDIR = ../bin
} else {
    DESTDIR = ../bin
}

OBJECTS_DIR = obj
MOC_DIR = moc

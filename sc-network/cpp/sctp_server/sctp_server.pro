#-------------------------------------------------
#
# Project created by QtCreator 2012-10-19T11:23:35
#
#-------------------------------------------------

QT       += core \
            network

QT       -= gui

TARGET = sctp_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    sctpClient.cpp \
    sctpServer.cpp  \
    sctpCommand.cpp \
    sctpStatistic.cpp

HEADERS += \
    sctpClient.h \
    sctpServer.h \
    sctpCommand.h \
    sctpTypes.h \
    sctpStatistic.h

CONFIG (debug, debug|release) {
    DESTDIR = ../../../bin
} else {
    DESTDIR = ../../../bin
}

INCLUDEPATH += ../../../sc-memory/src

unix {
    LIBS += $$DESTDIR/libsc_memory.so
}

OBJECTS_DIR = obj
MOC_DIR = moc


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
    sctpclient.cpp \
    sctpserver.cpp

CONFIG (debug, debug|release) {
    DESTDIR = ../bin
} else {
    DESTDIR = ../bin
}

unix {
    POST_TARGETDEPS += $$DESTDIR/libsctp.so
    LIBS += $$DESTDIR/libsctp.so
}

OBJECTS_DIR = obj
MOC_DIR = moc

HEADERS += \
    sctpclient.h \
    sctpserver.h

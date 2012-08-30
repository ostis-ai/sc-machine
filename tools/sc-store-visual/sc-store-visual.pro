#-------------------------------------------------
#
# Project created by QtCreator 2012-08-30T00:26:12
#
#-------------------------------------------------

QT       += core gui

TARGET = sc-store-visual
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    segmentview.cpp \
    segmentscene.cpp \
    segmentitem.cpp

HEADERS  += mainwindow.h \
    segmentview.h \
    segmentscene.h \
    segmentitem.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../sc-store/src
unix: LIBS += $$quote(-L$$DESTDIR) -lsc_store

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0


OBJECTS_DIR = obj
MOC_DIR = moc

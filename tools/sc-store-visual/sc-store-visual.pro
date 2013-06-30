#-------------------------------------------------
#
# Project created by QtCreator 2012-08-30T00:26:12
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = sc-store-visual
TEMPLATE = app
DESTDIR = ../../bin

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

INCLUDEPATH += ../../sc-memory/src
unix {
    LIBS += $$quote(-L$$DESTDIR) -lsc_memory
}


win32 {
    INCLUDEPATH += "../../glib/include/glib-2.0"
    INCLUDEPATH += "../../glib/lib/glib-2.0/include"

    #POST_TARGETDEPS += "../glib/lib/glib-2.0.lib"
    LIBS += "../../glib/lib/glib-2.0.lib"
    LIBS += $$quote(-L$$DESTDIR) -lsc_memoryd
}

OBJECTS_DIR = obj
MOC_DIR = moc

TEMPLATE = app
DESTDIR = ../../bin

INCLUDEPATH += ../../sc-memory/src
unix {
    LIBS += $$quote(-L$$DESTDIR) -lsc_memory
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}

DEFINES += QT_COMPILATION

SOURCES += \
    test.cpp

win32 {
    CONFIG += qt console
    INCLUDEPATH += "../../glib/include/glib-2.0"
    INCLUDEPATH += "../../glib/lib/glib-2.0/include"

    POST_TARGETDEPS += ../../glib/lib/glib-2.0.lib
    LIBS += ../../glib/lib/glib-2.0.lib
    LIBS += $$quote(-L$$DESTDIR) -lsc_memoryd
}

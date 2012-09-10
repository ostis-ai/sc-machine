TEMPLATE = app
DESTDIR = ../../bin

INCLUDEPATH += ../../sc-store/src
unix: LIBS += $$quote(-L$$DESTDIR) -lsc_store

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

SOURCES += \
    test.cpp

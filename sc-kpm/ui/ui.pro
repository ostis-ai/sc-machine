TEMPLATE = lib
BINDIR = ../../bin
DESTDIR = $$quote($$BINDIR)/extensions

INCLUDEPATH += ../../sc-memory/src/sc-store

unix {
    LIBS += $$quote(-L$$BINDIR) -lsc_memory
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}

HEADERS += \
    ui.h

SOURCES += \
    ui.c

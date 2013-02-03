TEMPLATE = lib
BINDIR = ../../bin
DESTDIR = $$quote($$BINDIR)/extensions

OBJECTS_DIR = obj
MOC_DIR = moc

INCLUDEPATH += ../../sc-memory/src

unix {
    LIBS += $$quote(-L$$BINDIR) -lsc_memory
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}

HEADERS += \
    search.h \
    search_functions.h \
    search_operations.h

SOURCES += \
    search.c \
    search_functions.c \
    search_operations.c

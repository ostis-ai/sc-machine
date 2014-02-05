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
    merge.h \
    merge_keynodes.h \
    merge_agents.h \
    merge_defines.h \
    merge_utils.h \
    agents/merge_set.h

SOURCES += \
    merge.c \
    merge_keynodes.c \
    merge_utils.c \
    agents/merge_set.c

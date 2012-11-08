TEMPLATE = lib
TARGET = $$qtLibraryTarget(sc_memory)

DESTDIR = ../bin

OBJECTS_DIR = obj
MOC_DIR = moc

HEADERS += \
    src/sc_memory.h \
    src/sc-store/sc_link_helpers.h \
    src/sc-store/sc_iterator.h \
    src/sc-store/sc_fs_storage.h \
    src/sc-store/sc_element.h \
    src/sc-store/sc_defines.h \
    src/sc-store/sc_config.h \
    src/sc-store/sc_types.h \
    src/sc-store/sc_stream_private.h \
    src/sc-store/sc_stream_file.h \
    src/sc-store/sc_stream.h \
    src/sc-store/sc_store.h \
    src/sc-store/sc_storage.h \
    src/sc-store/sc_segment.h \
    src/sc-store/sc_stream_memory.h \
    src/sc-store/sc_event.h

SOURCES += \
    src/sc_memory.c \
    src/sc-store/sc_link_helpers.c \
    src/sc-store/sc_iterator.c \
    src/sc-store/sc_fs_storage.c \
    src/sc-store/sc_element.c \
    src/sc-store/sc_stream_file.c \
    src/sc-store/sc_stream.c \
    src/sc-store/sc_storage.c \
    src/sc-store/sc_segment.c \
    src/sc-store/sc_stream_memory.c \
    src/sc-store/sc_event.c

win32 {
    INCLUDEPATH += "../glib/include/glib-2.0"
    INCLUDEPATH += "../glib/lib/glib-2.0/include"

    POST_TARGETDEPS += ../glib/lib/glib-2.0.lib
    LIBS += ../glib/lib/glib-2.0.lib
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}

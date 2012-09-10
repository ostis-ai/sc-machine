TEMPLATE = lib
TARGET = $$qtLibraryTarget(sc_store)

DESTDIR = ../bin

OBJECTS_DIR = obj
MOC_DIR = moc

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

HEADERS += \
    src/sc_types.h \
    src/sc_store.h \
    src/sc_storage.h \
    src/sc_segment.h \
    src/sc_iterator.h \
    src/sc_fs_storage.h \
    src/sc_element.h \
    src/sc_defines.h \
    src/sc_config.h \
    src/sc_link_helpers.h \
    src/sc_stream.h \
    src/sc_stream_private.h \
    src/sc_stream_file.h \
    src/sc_memory.h

SOURCES += \
    src/sc_storage.c \
    src/sc_segment.c \
    src/sc_iterator.c \
    src/sc_fs_storage.c \
    src/sc_element.c \
    src/sc_link_helpers.c \
    src/sc_stream.c \
    src/sc_stream_file.c \
    src/sc_memory.c



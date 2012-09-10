TEMPLATE = lib
TARGET = $$qtLibraryTarget(sc_store)

DESTDIR = ../bin

OBJECTS_DIR = obj
MOC_DIR = moc

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

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
    src/sc-store/sc_segment.h

SOURCES += \
    src/sc_memory.c \
    src/sc-store/sc_link_helpers.c \
    src/sc-store/sc_iterator.c \
    src/sc-store/sc_fs_storage.c \
    src/sc-store/sc_element.c \
    src/sc-store/sc_stream_file.c \
    src/sc-store/sc_stream.c \
    src/sc-store/sc_storage.c \
    src/sc-store/sc_segment.c




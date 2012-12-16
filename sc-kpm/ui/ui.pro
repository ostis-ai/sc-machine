TEMPLATE = lib
BINDIR = ../../bin
DESTDIR = $$quote($$BINDIR)/extensions

INCLUDEPATH += ../../sc-memory/src

OBJECTS_DIR = obj
MOC_DIR = moc

unix {
    LIBS += $$quote(-L$$BINDIR) -lsc_memory
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}

HEADERS += \
    ui.h \
    translators/ui_translator_sc2scs.h \
    ui_translators.h \
    ui_commands.h \
    ui_keynodes.h

SOURCES += \
    ui.c \
    translators/ui_translator_sc2scs.c \
    ui_translators.c \
    ui_commands.c \
    ui_keynodes.c

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
    translators/uiSc2ScsTranslator.h \
    uiCommands.h \
    uiKeynodes.h \
    uiTypes.h \
    uiTranslators.h \
    uiPrecompiled.h

SOURCES += \
    ui.cpp \
    translators/uiSc2ScsTranslator.cpp \
    uiCommands.cpp \
    uiTranslators.cpp \
    uiKeynodes.cpp \
    uiTypes.cpp

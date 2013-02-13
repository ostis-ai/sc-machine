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
    uiCommands.h \
    uiKeynodes.h \
    uiTypes.h \
    uiTranslators.h \
    uiPrecompiled.h \
    translators/uiSc2SCgJsonTranslator.h \
    translators/uiSc2SCnJsonTranslator.h \
    translators/uiTranslatorFromSc.h \
    translators/uiSc2ScsJsonTranslator.h \
    uiDefines.h

SOURCES += \
    ui.cpp \
    uiCommands.cpp \
    uiTranslators.cpp \
    uiKeynodes.cpp \
    uiTypes.cpp \
    translators/uiSc2SCgJsonTranslator.cpp \
    translators/uiSc2SCnJsonTranslator.cpp \
    translators/uiTranslatorFromSc.cpp \
    translators/uiSc2ScsJsonTranslator.cpp

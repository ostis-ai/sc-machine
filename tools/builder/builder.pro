TEMPLATE = app
CONFIG += console
CONFIG -= qt

DESTDIR = bin

OBJECTS_DIR = obj
MOC_DIR = moc

# antlr
antrl.target = scsParser
antrl.commands = antlr3 grammar/scs.g -fo src/parser
QMAKE_EXTRA_TARGETS += antrl

PRE_TARGETDEPS += scsParser

unix {
    CONFIG += link_pkgconfig
    LIBS += -lantlr3c -lboost_program_options -lboost_filesystem -lboost_system
}

SOURCES += \
    src/parser/scsParser.c \
    src/parser/scsLexer.c \
    src/builder.cpp \
    src/main.cpp \
    src/version.cpp \
    src/utils.cpp

HEADERS += \
    src/parser/scsParser.h \
    src/parser/scsLexer.h \
    src/builder.h \
    src/types.h \
    src/version.h \
    src/utils.h

OTHER_FILES += \
    grammar/scs.g


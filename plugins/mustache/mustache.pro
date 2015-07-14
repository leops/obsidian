TEMPLATE      = lib
CONFIG       += plugin c++14 no_keywords
QT           += core network
INCLUDEPATH  += ../../include

HEADERS       = \
    mustache.hpp \
    parser.h
SOURCES       = \
    mustache.cpp \
    parser.cpp \
    ../../src/global.cpp

TARGET        = $$qtLibraryTarget(view_mustache)
DESTDIR       = ../../../../site/plugins

DISTFILES += \
    mustache.json

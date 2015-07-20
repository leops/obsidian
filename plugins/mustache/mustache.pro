TEMPLATE      = lib
CONFIG       += plugin c++14 no_keywords
QT           += core network websockets
INCLUDEPATH  += ../../include

HEADERS       = \
    mustache.hpp \
    parser.h
SOURCES       = \
    mustache.cpp \
    parser.cpp

TARGET        = $$qtLibraryTarget(view_mustache)
DESTDIR       = ../../../../bassdrop/plugins

DISTFILES += \
    mustache.json

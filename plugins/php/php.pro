TEMPLATE      = lib
CONFIG       += plugin c++14 no_keywords
QT           += core network websockets
INCLUDEPATH  += ../../include

HEADERS       = \
    php.hpp \
    ph7.h
SOURCES       = \
    php.cpp

TARGET        = $$qtLibraryTarget(ctrl_php)
DESTDIR       = ../../../../site/plugins

DISTFILES += \
    php.json

RESOURCES +=

TEMPLATE      = lib
CONFIG       += plugin c++14 no_keywords
QT           += core network
INCLUDEPATH  += ../../include

HEADERS       = \
    assets.hpp
SOURCES       = \
    assets.cpp \
    ../../src/global.cpp

TARGET        = $$qtLibraryTarget(ctrl_assets)
DESTDIR       = ../../../../site/plugins

DISTFILES += \
    assets.json

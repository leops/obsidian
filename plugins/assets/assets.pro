TEMPLATE      = lib
CONFIG       += plugin c++14 no_keywords
QT           += core network websockets
INCLUDEPATH  += ../../include

HEADERS       = \
    assets.hpp
SOURCES       = \
    assets.cpp

TARGET        = $$qtLibraryTarget(ctrl_assets)
DESTDIR       = ../../../../bassdrop/plugins

DISTFILES += \
    assets.json

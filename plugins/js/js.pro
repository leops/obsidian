TEMPLATE      = lib
CONFIG       += plugin c++14 no_keywords
QT           += core script network websockets
INCLUDEPATH  += ../../include

HEADERS       = \
    js.hpp
SOURCES       = \
    js.cpp

TARGET        = $$qtLibraryTarget(ctrl_js)
DESTDIR       = ../../../../bassdrop/plugins

DISTFILES += \
    js.json

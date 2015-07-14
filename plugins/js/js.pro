TEMPLATE      = lib
CONFIG       += plugin c++14 no_keywords
QT           += core script network sql
INCLUDEPATH  += ../../include

HEADERS       = \
    js.hpp
SOURCES       = \
    js.cpp \
    ../../src/global.cpp

TARGET        = $$qtLibraryTarget(ctrl_js)
DESTDIR       = ../../../../site/plugins

DISTFILES += \
    js.json

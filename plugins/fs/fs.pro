TEMPLATE      = lib
CONFIG       += plugin c++14
QT           += core network websockets
INCLUDEPATH  += ../../include

HEADERS       = \
    fs.hpp
SOURCES       = \
    fs.cpp

TARGET        = $$qtLibraryTarget(mdl_fs)
DESTDIR       = ../../../../bassdrop/plugins

DISTFILES += \
    fs.json

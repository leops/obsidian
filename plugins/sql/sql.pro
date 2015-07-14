TEMPLATE      = lib
CONFIG       += plugin c++14
QT           += core network sql
INCLUDEPATH  += ../../include

HEADERS       = \
    sql.hpp
SOURCES       = \
    sql.cpp \
    ../../src/global.cpp

TARGET        = $$qtLibraryTarget(mdl_sql)
DESTDIR       = ../../../../site/plugins

DISTFILES += \
    sql.json

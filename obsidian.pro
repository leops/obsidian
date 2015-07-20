#-------------------------------------------------
#
# Project created by QtCreator 2015-03-11T08:59:48
#
#-------------------------------------------------

QT       += core network websockets
QT       -= gui

TARGET = obsidian

CONFIG   += console c++14
CONFIG   -= app_bundle

TEMPLATE = app
INCLUDEPATH += include

SOURCES += \
    src\main.cpp \
    src\httprequest.cpp \
    src\httpresponse.cpp \
    src\obsidian.cpp \
    src\router.cpp \
    src/wsrequest.cpp \
    src/wsresponse.cpp \
    src/abstractresponse.cpp

HEADERS += \
    include\httprequest.hpp \
    include\httpresponse.hpp \
    include\controller.hpp \
    include\model.hpp \
    include\view.hpp \
    include\obsidian.hpp \
    include\router.hpp \
    include/wsrequest.hpp \
    include/wsresponse.hpp \
    include/abstractrequest.hpp \
    include/abstractresponse.hpp

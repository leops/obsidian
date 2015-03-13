#-------------------------------------------------
#
# Project created by QtCreator 2015-03-11T08:59:48
#
#-------------------------------------------------

QT       += core network script sql
QT       -= gui

TARGET = obsidian

CONFIG   += console c++14
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    httprequest.cpp \
    httpresponse.cpp \
    controller.cpp \
    model.cpp \
    global.cpp \
    view.cpp \
    obsidian.cpp

HEADERS += \
    httprequest.hpp \
    httpresponse.hpp \
    controller.hpp \
    model.hpp \
    global.hpp \
    view.hpp \
    obsidian.hpp

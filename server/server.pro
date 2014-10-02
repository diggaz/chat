
QT       += core network sql

QT       -= gui

TARGET = chat_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ./ \
    ../common

SOURCES += \
    server_main.cpp \
    server.cpp \
    session.cpp \
    room.cpp \
    db_manager.cpp \
    ../common/packet.cpp

HEADERS += \
    server.h \
    session.h \
    room.h \
    db_connection.h \
    db_manager.cpp \
    ../common/packet.h

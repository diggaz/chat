
QT       += core network testlib sql

QT       -= gui

TARGET   = unittest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ./ \
    ../ \
    ../common \
    ../server \
    ../client

SOURCES += main.cpp \
    qunittest.cpp \
    ../common/packet.cpp \
    ../server/server.cpp \
    ../server/session.cpp \
    ../server/room.cpp \
    ../server/db_manager.cpp \
    ../client/client.cpp

HEADERS += \
    ../common/packet.h \
    ../server/user.h \
    ../server/server.h \
    ../server/session.h \
    ../server/room.h \
    ../server/db_connection.h \
    ../server/db_manager.h \
    ../client/client.h

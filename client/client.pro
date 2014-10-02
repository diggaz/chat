
QT += qml quick core network

TARGET = chat_client

TEMPLATE = app

INCLUDEPATH += ./ \
    ../common

SOURCES += \
    ../common/packet.cpp \
    client_main.cpp \
    client.cpp \
    chatclient.cpp

HEADERS += \
    ../common/packet.h \
    client.h \
    chatclient.h

RESOURCES += \
    views.qrc

OTHER_FILES += \
    main.qml \
    ./content/LoginView.qml \
    ./content/ChatView.qml

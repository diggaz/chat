#include <QGuiApplication>
#include <qqml.h>
#include <QQmlFileSelector>
#include <QtQuick/QQuickView>
#include "chatclient.h"

int main(int argc, char *argv[])
{   
    QGuiApplication app(argc,argv);

    qmlRegisterType<ChatClient>("Chat", 1, 0, "ChatClient");

    QQuickView view;
    view.connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    new QQmlFileSelector(view.engine(), &view);
    view.setSource(QUrl("qrc:///views/main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return app.exec();
}

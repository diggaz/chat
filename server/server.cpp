#include <QtNetwork>
#include <QDebug>
#include <QCoreApplication>
#include "server.h"
#include "session.h"
#include "db_connection.h"
#include "db_manager.h"

server::server(QObject *parent) 
    :  QObject(parent)
    , _tcp_server(0), _network_session(0)
{
    auto db = std::make_shared<db_connection_sqlite>(QString("temp.sqlite"));
    _db_manager = std::make_shared<db_manager>(db);
}

server::~server()
{
    _room.leave(_db_manager.get());
}

bool server::initialize()
{
    if (!_db_manager->initialize())
    {
        qDebug() << "can't initialize database";
        return false;
    }
    _room.join(_db_manager.get());

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("chat_server"));
        settings.beginGroup(QLatin1String("network"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        _network_session = new QNetworkSession(config, this);
        connect(_network_session, SIGNAL(opened()), this, SLOT(session_opened()));

        qDebug()<< tr("Opening network session.");
        _network_session->open();
    }
    else
    {
        session_opened();
    }

    return true;
}


void server::session_opened()
{
    // Save the used configuration
    if (_network_session)
    {
        QNetworkConfiguration config = _network_session->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = _network_session->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("chat_server"));
        settings.beginGroup(QLatin1String("network"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    _tcp_server = new QTcpServer(this);
    if (!_tcp_server->listen(QHostAddress::Any, 22000)) {
        qDebug()<< tr("Unable to start the server: %1.")
                   .arg(_tcp_server->errorString());
        qApp->quit();
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    qDebug() << tr("The server is running on\nIP: %1\nport: %2\n")
                .arg(ipAddress).arg(_tcp_server->serverPort());

    connect(_tcp_server, SIGNAL(newConnection()), this, SLOT(start_new_session()));
}

void server::start_new_session()
{
    QTcpSocket *connection = _tcp_server->nextPendingConnection();
    session *new_session = new session(connection, &_room, _db_manager.get());
    connect(connection, SIGNAL(disconnected()),
            new_session, SLOT(deleteLater()));
    new_session->start();
}

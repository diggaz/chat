#include <QtNetwork>
#include <QDebug>
#include <QCoreApplication>
#include "client.h"
#include "packet.h"

client::client(QObject* parent) 
    :  QObject(parent)
    , _tcp_socket(0), networkSession(0)
{
}

void client::initialize()
{
    _tcp_socket = new QTcpSocket(this);
    connect(_tcp_socket, SIGNAL(readyRead()), this, SLOT(handle_read()));
    connect(_tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(display_error(QAbstractSocket::SocketError)));
    connect(_tcp_socket, SIGNAL(connected()), this, SLOT(handle_connected()));
    connect(_tcp_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(_tcp_socket, SIGNAL(disconnected()), this, SLOT(handle_disconnected()));
    connect(_tcp_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("chat_client"));
        settings.beginGroup(QLatin1String("network"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(session_opened()));

        networkSession->open();
    }
    else
    {
        session_opened();
    }
}

void client::session_opened()
{
    // Save the used configuration
    if (networkSession)
    {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("chat_client"));
        settings.beginGroup(QLatin1String("network"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }
}

void client::display_error(QAbstractSocket::SocketError socketError)
{
    QString error_message;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        error_message = tr("The host was not found. Please check the "
                           "host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        error_message = tr("The connection was refused by the peer.");
        break;
    default:
        error_message = tr("The following error occurred: %1.")
                .arg(_tcp_socket->errorString());
    }

    qDebug() << error_message;
    emit error(error_message);
}

void client::handle_read()
{
    QDataStream in(_tcp_socket);
    in.setVersion(QDataStream::Qt_4_0);

    while (_tcp_socket->bytesAvailable() > 0)
    {
        QByteArray data;
        in >> data;
        _data.append(data);
        packet_wrapper pw;
        if (pw.parse(_data))
        {
            _data.clear();
            auto packet = pw.get_packet();
            process_packet(packet);
        }
        else
        {
            qDebug() << "perhaps a chunk: " << _data.toHex();
        }
    }
}

void client::connect_to_server(const QString& host, quint16 port)
{
    _data.clear();
    _tcp_socket->abort();
    _tcp_socket->connectToHost(host, port);
}

bool client::send(const packet *msg)
{
    if (_tcp_socket->state() != QAbstractSocket::ConnectedState)
        return false;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)0;
    out << msg->data();
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    _tcp_socket->write(block);

    return true;
}

void client::handle_connected()
{
}

void client::handle_disconnected()
{
}

void client::process_packet(packet* packet_ptr)
{
    // qDebug() << __FUNCTION__ << " type:" << packet_ptr->type();

    switch(packet_ptr->type())
    {
    case USER_LOGIN_PACKET_TYPE:
    {
        user_login* msg = static_cast<user_login*>(packet_ptr);
        emit user_login_received(msg->name());
        break;
    }
    case USER_LOGOUT_PACKET_TYPE:
    {
        user_login* msg = static_cast<user_login*>(packet_ptr);
        emit user_logout_received(msg->name());
        break;
    }
    case SERVICE_PACKET_TYPE:
    {
        service_message* msg = static_cast<service_message*>(packet_ptr);
        process_service_packet(msg);
        break;
    }
    case CHAT_MESSAGE_PACKET_TYPE:
    {
        chat_message* msg = static_cast<chat_message*>(packet_ptr);
        emit message_received(msg->name(), msg->message(), QDateTime::fromTime_t(msg->timestamp()));
        break;
    }
    case USER_LIST_PACKET_TYPE:
    {
        user_list* msg = static_cast<user_list*>(packet_ptr);
        emit user_list_received(msg->users());
        break;
    }
    case HISTORY_LIST_PACKET_TYPE:
    {
        history_list* msg = static_cast<history_list*>(packet_ptr);
        
        // TODO: one method
        foreach(const history_list::history& h, msg->histories())
        {
            emit message_received(h.name, h.message, QDateTime::fromTime_t(h.timestamp));
        }

        break;
    }
    default:
        qDebug() << __FUNCTION__ << " packet wasn't processed: " << packet_ptr->type();
        break;
    }
}

bool client::login_with_name(const QString& name)
{
    if (name.isEmpty())
        return false;

    _name = name;
    if (_tcp_socket->state() != QAbstractSocket::ConnectedState)
        return false;

    user_login info(name);
    send(&info);

    return true;
}

const QString& client::name() const
{
    return _name;
}

void client::process_service_packet(service_message* msg)
{
    switch(msg->service_type())
    {
    case service_message::LOGIN_REQUIRED_TYPE:
    {
        login_with_name(_name);
        break;
    }
    case service_message::WELCOME_TYPE:
    {
        emit joined_chat();
        break;
    }
    default:
        qDebug() << __FUNCTION__ << " packet wasn't processed: " << msg->type();
        break;
    }
}

void client::get_history(const QDateTime& from, const int count)
{
    get_history_list ghl(from, count);
    send(&ghl);
}

void client::send_message(const QString& message)
{
    chat_message msg(message);
    send(&msg);
}

#include <QDebug>
#include "chatclient.h"
#include "client.h"

ChatClient::ChatClient(QObject *parent)
    : QObject(parent)
    , _name("Guest")
    , _host("127.0.0.1")
    , _port(22000)
{
    _client = std::make_shared<client>();
    _client->initialize();

    //connect(_client.get(), SIGNAL(connected()), this, SIGNAL(connected()));
    connect(_client.get(), SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(_client.get(), SIGNAL(joined_chat()), this, SIGNAL(joinedChat()));
    connect(_client.get(), SIGNAL(error(const QString&)), this, SIGNAL(error(const QString&)));
    connect(_client.get(), SIGNAL(message_received(const QString&, const QString&, const QDateTime&)),
            SIGNAL(messageReceived(const QString&, const QString&, const QDateTime&)));
    connect(_client.get(), SIGNAL(server_info(const QString&)), this, SIGNAL(serverInfo(const QString&)));
    connect(_client.get(), SIGNAL(user_list_received(const QStringList&)), this, SIGNAL(userListReceived(const QStringList&)));
    connect(_client.get(), SIGNAL(user_login_received(const QString&)), this, SIGNAL(userLoginReceived(const QString&)));
    connect(_client.get(), SIGNAL(user_logout_received(const QString&)), this, SIGNAL(userLogoutReceived(const QString&)));

    connect(_client.get(), SIGNAL(connected()), this, SLOT(onConnected()));
    connect(_client.get(), SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(_client.get(), SIGNAL(error(const QString&)), this, SLOT(onError(const QString&)));
}

QString ChatClient::name() const
{
    return _name;
}

void ChatClient::setName(const QString& name)
{
    if (_name != name)
    {
        _name = name;
        emit nameChanged();
    }
}

QString ChatClient::host() const
{
    return _host;
}

void ChatClient::setHost(const QString& host)
{
    if (_host != host)
    {
        _host = host;
        emit hostChanged();
    }
}

quint16 ChatClient::port() const
{
    return _port;
}

void ChatClient::setPort(const quint16& port)
{
    if (_port != port)
    {
        _port = port;
        emit portChanged();
    }
}

void ChatClient::connectToServer()
{
    _client->connect_to_server(_host, _port);
}

void ChatClient::sendMessage(const QString& message)
{
    _client->send_message(message);
}

void ChatClient::requestHistory(const QDateTime datetime)
{
    _client->get_history(datetime, 10);
}

void ChatClient::onConnected()
{
    _client->login_with_name(_name);
}

void ChatClient::onDisconnected()
{
}

void ChatClient::onError(const QString &/*error*/)
{
}

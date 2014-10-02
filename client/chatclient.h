#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QDateTime>
#include <memory>

class client;

class ChatClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
public:
    explicit ChatClient(QObject* parent = 0);

    QString name() const;
    void setName(const QString& name);

    QString host() const;
    void setHost(const QString& host);

    quint16 port() const;
    void setPort(const quint16& port);

    Q_INVOKABLE void connectToServer();
    Q_INVOKABLE void sendMessage(const QString& message);
    Q_INVOKABLE void requestHistory(const QDateTime datetime);

signals:
    void nameChanged();
    void hostChanged();
    void portChanged();
    void joinedChat();
    void disconnected();
    void userLoginReceived(const QString& name);
    void userLogoutReceived(const QString& name);
    void messageReceived(const QString& name, const QString& message, const QDateTime& dt);
    void serverInfo(const QString& message);
    void userListReceived(const QStringList& users);
    void error(const QString& errorMessage);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(const QString&);

private:
    QString _name;
    QString _host;
    quint16 _port;
    std::shared_ptr<client> _client;
};

#endif // CHATCLIENT_H

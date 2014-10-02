#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QAbstractSocket>

class QTcpSocket;
class QNetworkSession;
class packet;
class service_message;

class client : public QObject
{
    Q_OBJECT
public:
    explicit client(QObject* parent = nullptr);

    void initialize();
    void connect_to_server(const QString& host, quint16 port);
    bool login_with_name(const QString& name);
    const QString& name() const;
    void get_history(const QDateTime& from, const int count);
    void send_message(const QString& message);

private:
    bool send(const packet* msg);
    void process_packet(packet* packet_ptr);
    void process_service_packet(service_message* msg);

signals:
    void message_received(const QString& name, const QString& message, const QDateTime&);
    void connected();
    void disconnected();
    void joined_chat();
    void server_info(const QString& message);
    void user_list_received(const QStringList&);
    void user_login_received(const QString& name);
    void user_logout_received(const QString& name);
    void error(const QString& error_message);

public slots:
    void session_opened();
    void display_error(QAbstractSocket::SocketError socketError);
    void handle_read();
    void handle_connected();
    void handle_disconnected();

private:
    QTcpSocket *_tcp_socket;
    QNetworkSession *networkSession;
    QByteArray _data;
    QString _name;
};

#endif // CLIENT_H

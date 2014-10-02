#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <room.h>
#include <memory>

class QTcpServer;
class QNetworkSession;
class db_manager;

class server : public QObject
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    ~server();

    bool initialize();
signals:

public slots:
    void session_opened();
    void start_new_session();

private:
    QTcpServer *_tcp_server;
    QNetworkSession *_network_session;
    room _room;
    std::shared_ptr<db_manager> _db_manager;
};

#endif // SERVER_H

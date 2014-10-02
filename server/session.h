#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include "user.h"

class QTcpSocket;
class room;
class packet;
class db_manager;

class session
        : public QObject
        , public user
{
    Q_OBJECT
    enum state_t
    {
        AUTHORIZATION,
        CONNECTED
    };
public:
    session(QTcpSocket* client_socket, room* room_ptr, db_manager* db_manager_ptr);
    ~session();

    void start();

    // user interface
public:
    void send(const packet* msg);
    QString name() const;

private:
    void process_packet(packet* packet_ptr);

private slots:
    void handle_read();


private:
    QTcpSocket* _client_socket;
    room* _room;
    state_t _state;
    QString _name;
    QByteArray _data;
    db_manager* _db_manager;
};

#endif // SESSION_H

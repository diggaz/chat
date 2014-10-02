#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <time.h>
#include "room.h"
#include "session.h"
#include "packet.h"
#include "db_manager.h"

session::session(QTcpSocket* client_socket, room* room_ptr, db_manager* db_manager_ptr)
    : QObject(nullptr)
    , _client_socket(client_socket)
    , _room(room_ptr)
    , _db_manager(db_manager_ptr)
    , _state(AUTHORIZATION)
{
}

session::~session()
{
    _room->leave(this);
    delete _client_socket;
}

void session::start()
{
    connect(_client_socket, SIGNAL(readyRead()), SLOT(handle_read()));
}

void session::send(const packet* msg)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)0;
    out << msg->data();
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    _client_socket->write(block);
}

void session::handle_read()
{
    QDataStream in(_client_socket);
    in.setVersion(QDataStream::Qt_4_0);

    while (_client_socket->bytesAvailable() > 0)
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

QString session::name() const
{
    return _name;
}

void session::process_packet(packet* packet_ptr)
{
    if (_state == AUTHORIZATION)
    {
        if (packet_ptr->type() == USER_LOGIN_PACKET_TYPE)
        {
            user_login* msg = static_cast<user_login*>(packet_ptr);
            if (!msg->name().isEmpty())
            {
                _state = CONNECTED;

                service_message service_msg("welcome", service_message::WELCOME_TYPE);
                send(&service_msg);

                _name = msg->name();
                _room->join(this);
                return;
            }
        }

        service_message service_msg("login required", service_message::LOGIN_REQUIRED_TYPE);
        send(&service_msg);
    }
    else
    {
        switch(packet_ptr->type())
        {
        case CHAT_MESSAGE_PACKET_TYPE:
        {
            auto chat_msg = static_cast<chat_message*>(packet_ptr);
            chat_msg->set_name(_name); // override sender name
            chat_msg->set_timestamp(time(nullptr));
            _room->send(chat_msg);

            break;
        }
        case GET_HISTORY_LIST_PACKET_TYPE:
        {
            auto get_history_msg = static_cast<get_history_list*>(packet_ptr);
            const auto& history = _db_manager->get_history(get_history_msg->from_date(), get_history_msg->limit());
            history_list history_list_packet(history);
            send(&history_list_packet);

            break;
        }
        default:
            qDebug() << __FUNCTION__ << " packet wasn't processed: " << packet_ptr->type();
            break;
        }
    }
}

#include <QDataStream>
#include <QDebug>
#include "packet.h"

packet::packet(packet_type_t type)
    : _type(type)
{
}

QByteArray packet::data() const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)type();
    out << serialize();

    return block;
}

template<typename T>
packet* create_packet()
{
    return new T();
}

packet* packet_factory::new_packet(packet_type_t type)
{
    typedef packet* (*factory_method_t)();

    static bool init = false;
    static std::map<packet_type_t, factory_method_t> _factory_methods;
    if (!init) // some kind of registration
    {
        init = true;
        _factory_methods[USER_LOGIN_PACKET_TYPE] = &create_packet<user_login>;
        _factory_methods[USER_LOGOUT_PACKET_TYPE] = &create_packet<user_logout>;
        _factory_methods[SERVICE_PACKET_TYPE] = &create_packet<service_message>;
        _factory_methods[CHAT_MESSAGE_PACKET_TYPE] = &create_packet<chat_message>;
        _factory_methods[USER_LIST_PACKET_TYPE] = &create_packet<user_list>;
        _factory_methods[GET_HISTORY_LIST_PACKET_TYPE] = &create_packet<get_history_list>;
        _factory_methods[HISTORY_LIST_PACKET_TYPE] = &create_packet<history_list>;
    }

    auto find_it = _factory_methods.find(type);
    if (find_it == _factory_methods.end())
        return nullptr;

    return find_it->second();
}

bool packet_wrapper::parse(const QByteArray &bytes)
{
    quint8 type_block_size = sizeof(quint32);
    quint8 data_block_size = sizeof(quint32);

    if (bytes.size() < (type_block_size + data_block_size))
        return false;

    quint32 size = 0;
    quint32 type = UNKNOWN_PACKET_TYPE;

    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_4_0);
    in >> size;
    in >> type;

    if ((quint32)bytes.size() < (size + type_block_size))
        return false;

    QByteArray payload;
    in >> payload;
    if ((quint32)payload.size() < (size - type_block_size - sizeof(quint32)))
        return false;

    _msg = packet_factory::new_packet(static_cast<packet_type_t>(type));
    if (!_msg)
        return false;

    return _msg->deserialize(payload);
}

QByteArray service_message::serialize() const 
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint8)_service_type;
    out << _message;

    return bytes;
}

bool service_message::deserialize(const QByteArray& bytes)
{
    if (bytes.size() < 1)
        return false;
    quint8 service_type = 0;
    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_4_0);
    in >> (quint8)service_type;
    in >> _message;
    _service_type = static_cast<service_type_t>(service_type);

    return true;
}

QByteArray user_login::serialize() const 
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << _name;

    return bytes;
}

bool user_login::deserialize(const QByteArray& bytes)
{
    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_4_0);
    in >> _name;
    return true;
}

QByteArray user_logout::serialize() const 
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << _name;

    return bytes;
}

bool user_logout::deserialize(const QByteArray& bytes)
{
    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_4_0);
    in >> _name;
    return true;
}

QByteArray chat_message::serialize() const 
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)_timestamp;
    out << _name;
    out << _message;

    return bytes;
}

bool chat_message::deserialize(const QByteArray& bytes)
{
    if (bytes.size() < sizeof(quint32))
        return false;

    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_4_0);
    in >> _timestamp;
    in >> _name;
    in >> _message;

    return true;
}

QByteArray user_list::serialize() const 
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << _users;

    return bytes;
}

bool user_list::deserialize(const QByteArray& bytes)
{
    QDataStream in(bytes);
    in >> _users;

    return true;
}

QByteArray get_history_list::serialize() const 
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << _from_dt;
    out << _history_limit;

    return bytes;
}

bool get_history_list::deserialize(const QByteArray& bytes)
{
    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_4_0);
    in >> _from_dt;
    in >> _history_limit;

    return true;
}

QByteArray history_list::serialize() const 
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    QList<QList<QVariant>> hist_list;
    foreach(const history& h, _history)
    {
        QList<QVariant> history_vars;
        history_vars << h.timestamp;
        history_vars << h.name;
        history_vars << h.message;
        hist_list << history_vars;
    }
    out << hist_list;

    return bytes;
}

bool history_list::deserialize(const QByteArray& bytes)
{
    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_4_0);
    QList<QList<QVariant>> hist_list;
    in >> hist_list;

    _history.clear();
    foreach(const QList<QVariant>& history_vars, hist_list)
    {
        if (history_vars.size() < 3)
            continue;

        auto it = history_vars.begin();
        history h;
        h.timestamp = it->toUInt(); ++it;
        h.name = it->toString(); ++it;
        h.message = it->toString(); ++it;
        _history.append(h);
    }

    return true;
}

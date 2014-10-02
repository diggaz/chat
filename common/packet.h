#ifndef PACKET_H
#define PACKET_H

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QList>
#include <map>

enum packet_type_t
{
    UNKNOWN_PACKET_TYPE,
    USER_LOGIN_PACKET_TYPE,
    USER_LOGOUT_PACKET_TYPE,
    SERVICE_PACKET_TYPE,
    CHAT_MESSAGE_PACKET_TYPE,
    USER_LIST_PACKET_TYPE,
    GET_HISTORY_LIST_PACKET_TYPE,
    HISTORY_LIST_PACKET_TYPE
};

class packet
{
public:
    packet(packet_type_t type);
    virtual ~packet(){}

    packet_type_t type() const { return _type; }

    QByteArray data() const;

    virtual QByteArray serialize() const = 0;
    virtual bool deserialize(const QByteArray& bytes) = 0;

protected:
    QByteArray _data;
    packet_type_t _type;
};

class packet_factory
{
public:
    static packet* new_packet(packet_type_t type);
};

class packet_wrapper
{
public:
    packet_wrapper() : _msg(nullptr){}
    ~packet_wrapper()
    {
        delete _msg;
    }

    packet* get_packet() const { return _msg; }

    bool parse(const QByteArray& bytes);

private:
    packet* _msg;
};

class service_message: public packet
{
public:
    enum service_type_t
    {
        UNSPECIFED_TYPE,
        LOGIN_REQUIRED_TYPE,
        WELCOME_TYPE,
        USER_LEAVE_TYPE
    };

    service_message()
        : packet(SERVICE_PACKET_TYPE)
        , _service_type(UNSPECIFED_TYPE) {}

    service_message(const QString& message, service_type_t srvc_type = UNSPECIFED_TYPE)
        : packet(SERVICE_PACKET_TYPE)
        , _message(message)
        , _service_type(srvc_type) {}

    QByteArray serialize() const override;
    bool deserialize(const QByteArray& bytes) override;

    const QString& message() const
    {
        return _message;
    }

    service_type_t service_type() const
    { return _service_type; }

private:
    QString _message;
    service_type_t _service_type;
};

class user_login: public packet
{
public:
    user_login(): packet(USER_LOGIN_PACKET_TYPE) {}
    user_login(const QString& name)
        : packet(USER_LOGIN_PACKET_TYPE)
        , _name(name){}

    QByteArray serialize() const override;
    bool deserialize(const QByteArray& bytes) override;

    const QString& name() const
    {
        return _name;
    }

private:
    QString _name;
};

class user_logout: public packet
{
public:
    user_logout(): packet(USER_LOGOUT_PACKET_TYPE) {}
    user_logout(const QString& name)
        : packet(USER_LOGOUT_PACKET_TYPE)
        , _name(name){}

    QByteArray serialize() const override;
    bool deserialize(const QByteArray& bytes) override;

    const QString& name() const
    {
        return _name;
    }

private:
    QString _name;
};

class chat_message: public packet
{
public:
    chat_message()
        : packet(CHAT_MESSAGE_PACKET_TYPE)
        , _timestamp(0) {}

    chat_message(const QString& message)
        : packet(CHAT_MESSAGE_PACKET_TYPE)
        , _message(message)
        , _timestamp(0) {}

    QByteArray serialize() const override;
    bool deserialize(const QByteArray& bytes) override;

    const QString& message() const
    {
        return _message;
    }

    void set_name(const QString& name)
    {
        _name = name;
    }

    const QString& name() const
    {
        return _name;
    }

    void set_timestamp(quint32 time)
    {
        _timestamp = time;
    }

    qint32 timestamp() const
    {
        return _timestamp;
    }

private:
    QString _message;
    QString _name;
    qint32 _timestamp;
};

class user_list: public packet
{
public:
    user_list(): packet(USER_LIST_PACKET_TYPE) {}
    user_list(const QStringList& users)
        : packet(USER_LIST_PACKET_TYPE)
        , _users(users){}

    QByteArray serialize() const override;
    bool deserialize(const QByteArray& bytes) override;

    const QStringList& users() const
    {
        return _users;
    }

private:
    QStringList _users;
};

class get_history_list: public packet
{
public:
    get_history_list()
        : packet(GET_HISTORY_LIST_PACKET_TYPE)
        , _history_limit(0) {}
    get_history_list(const QDateTime from_dt, quint32 limit)
        : packet(GET_HISTORY_LIST_PACKET_TYPE)
        , _from_dt(from_dt)
        , _history_limit(limit){}

    QByteArray serialize() const override;
    bool deserialize(const QByteArray& bytes) override;

    const QDateTime& from_date() const
    {
        return _from_dt;
    }

    const quint32 limit() const
    {
        return _history_limit;
    }

private:
    QDateTime _from_dt;
    quint32 _history_limit;
};

class history_list: public packet
{
public:
    struct history
    {
        quint32 timestamp;
        QString name;
        QString message;
    };

    history_list(): packet(HISTORY_LIST_PACKET_TYPE) {}
    history_list(const QList<history>& histories)
        : packet(HISTORY_LIST_PACKET_TYPE)
        , _history(histories){}

    QByteArray serialize() const override;
    bool deserialize(const QByteArray& bytes) override;

    const QList<history>& histories() const
    {
        return _history;
    }

private:
    QList<history> _history;
};
#endif  // _PACKET_HPP_

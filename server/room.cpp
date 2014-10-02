#include <QDataStream>
#include <QDebug>
#include "room.h"
#include "session.h"
#include "packet.h"

room::room()
{
}

void room::join(user* user_ptr)
{
    if (_users.count(user_ptr))
        return;

    // send to others 'user login'
    user_login msg(user_ptr->name());
    send(&msg);

    _users.insert(user_ptr);
    qDebug() << __FUNCTION__ << QString("new user \"%1\" joined").arg(user_ptr->name());

    // send user list to new client
    {
        QStringList users;
        for (auto it = _users.cbegin(); it != _users.cend(); ++it)
        {
            if ((*it)->hidden())
                continue;
            users << (*it)->name();
        }

        user_list list(users);
        user_ptr->send(&list);
    }
}

void room::leave(user* user_ptr)
{
    qDebug() << QString("user \"%1\" leave chat").arg(user_ptr->name());
    _users.erase(user_ptr);

    // send to other user logout
    user_logout msg(user_ptr->name());
    send(&msg);
}

void room::send(const packet* msg)
{
    //qDebug() << __FUNCTION__ << "type: " << msg->type();
    for (auto it = _users.cbegin(); it != _users.cend(); ++it)
        (*it)->send(msg);
}

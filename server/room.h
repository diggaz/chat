#ifndef ROOM_H
#define ROOM_H

#include <set>

class packet;
class user;

class room
{
public:
    room();

    void join(user *user_ptr);
    void leave(user *user_ptr);
    void send(const packet *msg);

private:
    std::set<user*> _users;
};

#endif // ROOM_H

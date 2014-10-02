#ifndef _USER_HPP
#define _USER_HPP

#include <QString>

class packet;

class user
{
public:
    virtual ~user() {}
    virtual void send(const packet* msg) = 0;
    virtual QString name() const = 0;
    virtual bool hidden() const { return false; }
};

#endif  // _USER_HPP

#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <memory>
#include "user.h"
#include "packet.h"

class db_connection;

class db_manager: public user
{
public:
    db_manager(std::shared_ptr<db_connection> db);

    bool initialize();

    // user
    void send(const packet* msg) override;
    QString name() const override;
    bool hidden() const override { return true; }

    QList<history_list::history> get_history(const QDateTime& dt, int limit = 10);

private:
    std::shared_ptr<db_connection> _db_connection;
};

#endif // DB_MANAGER_H

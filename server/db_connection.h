#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <QSqlDatabase>
#include <QUuid>

class db_connection
{
public:
    db_connection(
            const QString& db_name,
            const QString& db_type,
            const QString& host = "",
            const QString& user_name = "",
            const QString& password = "",
            int port = 0,
            const QString& options = "")
        : _db(nullptr)
    {
        _db = new QSqlDatabase(QSqlDatabase::addDatabase(db_type, QUuid::createUuid().toString()));
        _db->setHostName(host);
        _db->setDatabaseName(db_name);
        _db->setUserName(user_name);
        _db->setPassword(password);
        _db->setConnectOptions(options);
        if (port)
            _db->setPort(port);
        _db->open();
    }

    ~db_connection()
    {
        QString connection_name = _db->connectionName();
        _db->close();
        delete _db;
        QSqlDatabase::removeDatabase(connection_name);
    }

    QSqlDatabase* db() const
    {
        return _db;
    }

private:
    QSqlDatabase* _db;
    db_connection(const db_connection&);
    db_connection& operator=(const db_connection&);
};

class db_connection_sqlite: public db_connection
{
public:
    db_connection_sqlite(const QString& db_name)
        : db_connection(db_name, "QSQLITE")
    {
    }
};

class db_connection_psql: public db_connection
{
public:
    db_connection_psql(
            const QString& db_name,
            const QString& host,
            const QString& user_name,
            const QString& password,
            int port = 0,
            const QString& options = "")
        : db_connection(db_name, "QPSQL", host, user_name, password, port, options)
    {
    }
};

#endif // DB_CONNECTION_H

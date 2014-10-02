#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QStringList>
#include "db_connection.h"
#include "db_manager.h"
#include "packet.h"

db_manager::db_manager(std::shared_ptr<db_connection> db) 
    : _db_connection(db)
{
}

bool db_manager::initialize()
{
    if(!_db_connection->db()->isValid())
        return false;

    if(!_db_connection->db()->isOpen())
    {
        qDebug() << _db_connection->db()->lastError().text();
        return false;
    }

    QStringList query_list;
    if (_db_connection->db()->driverName() == "QPSQL")
    {
        query_list << QString("CREATE DATABASE IF NOT EXISTS '%1'").arg( _db_connection->db()->databaseName());
        query_list << "CREATE TABLE IF NOT EXISTS history (time timestamp without time zone, name text NOT NULL, data text NOT NULL)";
    }
    else if (_db_connection->db()->driverName() == "QSQLITE")
    {
        query_list << "CREATE TABLE IF NOT EXISTS history (time DATETIME not null, name VARCHAR(255) not null, data BLOB null)";
        query_list << "CREATE INDEX IF NOT EXISTS history_time_idx ON history (time)";
    }

    if(!_db_connection->db()->transaction())
    {
        qDebug() << _db_connection->db()->lastError().text();
        return false;
    }

    foreach(const QString& query_string, query_list)
    {
        QSqlQuery query(*_db_connection->db());
        if (!query.exec(query_string))
        {
            qDebug() << query.lastError().text();
            return false;
        }
    }

    if(!_db_connection->db()->commit())
    {
        qDebug() << _db_connection->db()->lastError().text();
        return false;
    }

    return true;
}

void db_manager::send(const packet* msg)
{
    if (msg->type() == CHAT_MESSAGE_PACKET_TYPE)
    {
        const chat_message* chat_msg = static_cast<const chat_message*>(msg);

        QString query_string("INSERT INTO history (time, name, data) VALUES(:time, :name, :data);");
        QSqlQuery query(*_db_connection->db());
        query.prepare(query_string);
        query.bindValue(":time", QDateTime::fromTime_t(chat_msg->timestamp()));
        query.bindValue(":name", chat_msg->name());
        auto data = chat_msg->message().toUtf8();
        query.bindValue(":data", data);
        if (!query.exec())
        {
            qDebug() << query.lastError().text();
            return;
        }
    }
}

QString db_manager::name() const
{
    static QString name_str("database manager"); // hardcoded
    return name_str;
}

QList<history_list::history> db_manager::get_history(const QDateTime& dt, int limit /*= 10*/)
{
    QList<history_list::history> histories;

    QString query_string("SELECT * FROM history WHERE time < :time_from ORDER BY time DESC LIMIT :limit;");
    QSqlQuery query(*_db_connection->db());
    query.prepare(query_string);
    query.bindValue(":time_from", dt);
    query.bindValue(":limit", limit);
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return histories;
    }

    QSqlRecord record = query.record();
    int time_index = record.indexOf("time");
    int name_index = record.indexOf("name");
    int data_index = record.indexOf("data");
    while(query.next())
    {
        record = query.record();
        history_list::history history;
        history.timestamp = record.value(time_index).toDateTime().toTime_t();
        history.name = record.value(name_index).toString();
        history.message = QString::fromUtf8(record.value(data_index).toByteArray());
        histories << history;
    }

    return histories;
}


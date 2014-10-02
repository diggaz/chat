#include <QtTest/QtTest>
#include <QDebug>
#include "server.h"
#include "client.h"
#include "packet.h"

class test_server: public QObject
{
    Q_OBJECT
private slots:
    void connect();
};

class client_handle: public QObject
{
    Q_OBJECT
public:
    client_handle(client* client_ptr, QString name)
        : _client(client_ptr), _name(name)
    {
        connect(client_ptr, SIGNAL(connected()), this, SLOT(on_connected()));
        connect(client_ptr, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
        connect(client_ptr, SIGNAL(joined_chat()), this, SLOT(on_joined_chat()));
        connect(client_ptr, SIGNAL(message_received(const QString&, const QString&, const QDateTime&)),
                SLOT(on_message_received(const QString&, const QString&, const QDateTime&)));
        connect(client_ptr, SIGNAL(server_info(const QString&)), this, SLOT(on_server_info(const QString&)));
        connect(client_ptr, SIGNAL(user_list_received(const QStringList&)), this, SLOT(on_user_list_received(const QStringList&)));
        connect(client_ptr, SIGNAL(user_login_received(const QString&)), this, SLOT(on_user_login_received(const QString&)));
        connect(client_ptr, SIGNAL(user_logout_received(const QString&)), this, SLOT(on_user_logout_received(const QString&)));
    }

private slots:
    void on_connected()
    {
        _client->login_with_name(_name);
    }

    void on_disconnected()
    {
    }

    void on_joined_chat()
    {
        _client->send_message("message");
    }

    void on_user_login_received(const QString& name)
    {
        qDebug() << "chat:" <<_name <<" user login: " << name;
    }

    void on_user_logout_received(const QString& name)
    {
        qDebug() << "chat:" <<_name <<" user logout: " << name;
    }

    void on_message_received(const QString& name, const QString& message, const QDateTime& dt)
    {
        qDebug() << "chat:" <<_name << " dt:" << dt.toString("hh:m:ss") <<" from name: " << name << " message: " << message;
    }

    void on_server_info(const QString& message)
    {
        qDebug() << "chat:" <<_name <<" server message: " << message;
    }

    void on_user_list_received(const QStringList& users)
    {
        qDebug() << "chat:" <<_name << " users:" << users;

        _client->get_history(QDateTime::currentDateTime(), 5);
    }

private:
    client* _client;
    QString _name;

};

void test_server::connect()
{
    server s;
    QVERIFY(s.initialize());

    QTest::qWait(100);

    {
        client client1;
        client1.initialize();
        client1.connect_to_server("127.0.0.1", 22000);
        client_handle test1(&client1, tr("test 1"));

        QTest::qWait(200);

        client client2;
        client2.initialize();
        client2.connect_to_server("127.0.0.1", 22000);
        client_handle test2(&client2, tr("test 2"));

        QTest::qWait(200);

        client client3;
        client3.initialize();
        client3.connect_to_server("127.0.0.1", 22000);
        client_handle test3(&client3, tr("test 3"));
    }

    QTest::qWait(50000);
}

QTEST_MAIN(test_server)
#include "qunittest.moc"

#include "gtest/gtest.h"
#include <map>
#include <QThread>
#include "server.h"
#include "client.h"

// вспомогательный класс
struct result_handler_test
{
    result_handler_test() : correct_results(0) {}

    void handler(int id, const std::string &result)
    {
        // в обработчик попадает результирующее сообщение
        // в нем идет проверка начальный результат, и конечный
        // в случае совпадения, увеличиваем счетчик корректных ответов
        auto it = result_map.find(id);
        if (it!= result_map.end() && it->second == result)
            correct_results++;
    }

    std::map<int, std::string> result_map;
    int correct_results;
};

TEST(server, connect)
{
    server s;
    s.initialize();

    QThread::sleep(1);

    client c;
    c.initialize();
    c.connect_to_server("127.0.0.1", 21000);

    QThread::sleep(50);
}

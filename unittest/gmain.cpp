#include <stdio.h>
#include <QCoreApplication>
#include <QThread>
#include "gtest/gtest.h"

class helper: public QThread
{
public:
    helper(int argc, char **argv): argc(argc), argv(*argv){}

    virtual void run()
    {
        testing::InitGoogleTest(&argc, &argv);
        int res = RUN_ALL_TESTS();
    }

private:
    int argc;
    char *argv;
};

GTEST_API_ int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    helper h(argc, argv);
    h.start();
//     testing::InitGoogleTest(&argc, argv);
//     //return RUN_ALL_TESTS();
//     int res = RUN_ALL_TESTS();

    //getchar();

    return app.exec();
}
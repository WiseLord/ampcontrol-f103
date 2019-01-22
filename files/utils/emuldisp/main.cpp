#include <QApplication>

#include "amp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Amp amp;

    amp.show();

    return a.exec();
}

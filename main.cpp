#include "packetsize.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a (argc, argv);
    PacketSize w;
    w.setAttribute(Qt::WA_DeleteOnClose);
    w.show();
    return a.exec();
}

#ifndef PACKETSIZE_H
#define PACKETSIZE_H

#include <QWidget>
//#include <QDebug>
#include <QProcess>

namespace Ui {
class PacketSize;
}

class PacketSize : public QWidget
{
    Q_OBJECT

public:
    explicit PacketSize(QWidget *parent = 0);
    ~PacketSize();

private:
    Ui::PacketSize *ui;
    Qt::SortOrder order[2];
    void keyPressEvent(QKeyEvent *event);
    QStringList *depends(QString nom);
    QProcess command;

private slots:
   void headerClick(int colNr);
   void	cellClicked(int row, int column);
   void on_scan_clicked();
   void on_search_textChanged(const QString &arg1);
};

#endif // PACKETSIZE_H

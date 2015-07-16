#include "packetsize.h"
#include "ui_packetsize.h"
#include <QString>
#include <QHeaderView>
#include <QSize>
#include <QDesktopWidget>
#include <QAbstractScrollArea>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPalette>
#include <QScrollBar>
#include <QKeyEvent>
#include <QTest>

PacketSize::PacketSize(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PacketSize)
{
    ui->setupUi(this);

    QHeaderView * vueHeader = new QHeaderView(Qt::Horizontal);
    vueHeader->setMinimumHeight(40);
    vueHeader->setSectionsMovable(true);
    vueHeader->setSectionsClickable(true);
    vueHeader->setSortIndicatorShown(true);
    ui->table->setHorizontalHeader(vueHeader);
    vueHeader -> setSectionResizeMode(QHeaderView::Stretch);
    QStringList header;
    header << QObject::tr("Name") << QObject::tr("Size");
    ui->table->horizontalHeader()->setSortIndicatorShown(true);
    connect(vueHeader, SIGNAL(sectionClicked(int)), this, SLOT(headerClick(int)));
    ui->table->setHorizontalHeaderLabels(header);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);

    QProcess command;
    command.setReadChannel(QProcess::StandardOutput);
    command.start("package-query -Qf%2;%n");
    command.waitForFinished();
    int row = 0;
    while(command.canReadLine())
    {
        QString line;
        QString length, name;
        line = command.readLine();
        line.chop(1);
        length = line.section(";", 0, 0);
        name=line.section(';', 1, 1);
        ui->table->setRowCount(row + 1);
        QTableWidgetItem *wname = new QTableWidgetItem(name);
        wname->setTextAlignment(Qt::AlignCenter);
        wname->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui->table->setItem(row, 0, wname);
        QTableWidgetItem *wlength = new QTableWidgetItem(QString("%1").arg(length.toInt(), 12));
        wlength->setTextAlignment(Qt::AlignCenter);
        wlength->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui->table->setItem(row, 1, wlength);
        row = row + 1;
    }
    connect(ui->table, SIGNAL(cellClicked(int,int)), this, SLOT(cellClicked(int,int)));
    ui->table->sortItems(1, Qt::DescendingOrder);
    order[0] = Qt::AscendingOrder;
    order[1] = Qt::AscendingOrder;
    int moveX = (QApplication::desktop()->width() - ui->table->width()) / 2;
    int moveY = (QApplication::desktop()->height() - ui->table->height() - 25) / 2;
    this->move(moveX, moveY);
}

PacketSize::~PacketSize()
{
    delete ui;
}

void PacketSize::headerClick(int colNr)
{
    ui->table->sortItems(colNr, order[colNr]);
    if(order[colNr] == Qt::AscendingOrder)
        order[colNr] = Qt::DescendingOrder;
    else
        order[colNr] = Qt::AscendingOrder;
}

QStringList *PacketSize::depends(QString nom)
{
    QStringList *listeDepends = new QStringList();
    QString line;
    QString temp;
    command.setReadChannel(QProcess::StandardOutput);
    temp = "yaourt -Q " + nom + " --depends";
    ui->command->setText(nom);
    //    connect(&command, SIGNAL(finished(int)), this, SLOT(on_process_end()));
    command.start(temp);
    QTest::qWait(10);
    command.waitForFinished();
    while(command.canReadLine())
    {
        line = command.readLine();
        line.remove(QRegExp(" .*"));
        line.remove(QRegExp(".*/"));
        listeDepends->append(line);
    }
    return listeDepends;
}

void PacketSize::cellClicked(int row, int column)
{
    QStringList *liste;
    QStandardItemModel model(20,2);
    liste = depends(ui->table->item(row,0)->text());
    int l = liste->size();
    if(l == 0)
    {
        ui->table->item(row, 0)->setBackgroundColor(QColor(182, 221, 232));
        ui->table->item(row, 1)->setBackgroundColor(QColor(182, 221, 232));
    }
    else
    {
        ui->table->item(row, 0)->setBackgroundColor(QColor(252, 213, 180));
        ui->table->item(row, 1)->setBackgroundColor(QColor(252, 213, 180));
        QTreeView *tree = new QTreeView();
        QStandardItemModel *model = new QStandardItemModel();
        for (int i = 0; i < l; i++)
        {
            QStandardItem *item = new QStandardItem(liste->at(i));
            model->setItem(i, item);
        }
        //        model->setHorizontalHeaderItem(0, new QStandardItem(ui->table->item(row,0)->text()));
        tree->header()->hide();
        tree->setModel(model);
        tree->setWindowTitle(ui->table->item(row,0)->text());
        tree->show();
    }
    free(liste);
}

void PacketSize::on_scan_clicked()
{
    ui->scan->setEnabled(false);
    ui->scan->setCheckable(false);
    for(int i = 0; i < ui->table->rowCount(); i++)
    {
        QStringList *liste;
        liste = depends(ui->table->item(i,0)->text());
        int l = liste->size();
        if(l == 0)
        {
            ui->table->item(i, 0)->setBackgroundColor(QColor(182, 221, 232));
            ui->table->item(i, 1)->setBackgroundColor(QColor(182, 221, 232));
        }
        else
        {
            ui->table->item(i, 0)->setBackgroundColor(QColor(252, 213, 180));
            ui->table->item(i, 1)->setBackgroundColor(QColor(252, 213, 180));
        }
        free(liste);
    }
    ui->command->setVisible(false);
    ui->scan->setVisible(false);

}

void PacketSize::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Home:
        ui->table->verticalScrollBar()->QScrollBar::setValue(0);
        break;
    case Qt::Key_End:
        ui->table->verticalScrollBar()->QScrollBar::setValue(ui->table->rowCount() - 1);
        break;
     default:
        qDebug() << event->key();
        QWidget::keyPressEvent(event);
    }

}

void PacketSize::on_search_textChanged(const QString &arg1)
{
    qDebug() << arg1;

    for(int i = 0; i < ui->table->rowCount(); i++)
        if (ui->table->item(i,0)->text().contains(ui->search->text(), Qt::CaseInsensitive))
        {
            ui->table->verticalScrollBar()->QScrollBar::setValue(i);
            break;
        }
}

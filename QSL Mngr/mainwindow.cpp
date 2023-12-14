#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addqsodialog.h"
#include "qsodetaildialog.h"

#include <QMessageBox>
#include "sqlite3.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&qsoDialog,SIGNAL(qsoInserted()),this,SLOT(qsoDialogReturned()));
    connect(&detailDialog,SIGNAL(qsoModifiedOrDeleted()),this,SLOT(qsoDialogReturned()));

    ui->qsoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    /*设置表格为整行选中*/
    ui->qsoTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList strs = {"QSO ID", "QSO Date", "UTC", "Callsign", "Frequency", "Mode", "RST Send", "RST Recv", "QSL Send", "QSL Recv", "Note"};
    ui->qsoTableWidget->setColumnCount(11);
    ui->qsoTableWidget->setHorizontalHeaderLabels(strs);
    ui->qsoTableWidget->setColumnWidth(0,70);
    ui->qsoTableWidget->setColumnWidth(1,70);
    ui->qsoTableWidget->setColumnWidth(2,40);
    ui->qsoTableWidget->setColumnWidth(3,80);
    ui->qsoTableWidget->setColumnWidth(4,70);
    ui->qsoTableWidget->setColumnWidth(5,60);
    ui->qsoTableWidget->setColumnWidth(6,80);
    ui->qsoTableWidget->setColumnWidth(7,80);
    ui->qsoTableWidget->setColumnWidth(8,80);
    ui->qsoTableWidget->setColumnWidth(9,80);
    ui->qsoTableWidget->setColumnWidth(10,300);
    refreshQSOTableWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshQSOTableWidget()
{
    ui->qsoTableWidget->clearContents();

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("QSLManagerSystem.db", &db);
    if(rc)
    {
        qDebug() << "Can't open database: " << zErrMsg;
        exit(0);
    }

    char **result, *errmsg;
    int nrow, ncolumn;

    QString cmd = "select * from QSO";
    if(!ui->callsignLineEdit->text().isEmpty())
    {
        cmd += " where call like \"%";
        cmd += ui->callsignLineEdit->text();
        cmd += "%\"";
    }
    //qDebug() << cmd;
    if (sqlite3_get_table(db, cmd.toUtf8().data(), &result, &nrow, &ncolumn, &errmsg) != 0)
    {
        qDebug() << "Can't open table QSO: " << errmsg;
        exit(0);
    }
    //qDebug() << nrow << ", "<< ncolumn;

    ui->qsoTableWidget->setRowCount(nrow);
    for(int i = 0; i < nrow; i++)
    {
        for(int j = 0; j < ncolumn; j++)
        {
            //qDebug() << result[(i + 1)* ncolumn + (j + 1)];
            QString str(result[(i + 1)* ncolumn + j]);
            if(j==0)str=QString("%1").arg(str, 8, '0');
            if(j==8)
            {
                if(!str.isEmpty())str=QString("%1").arg(str, 8, '0');
            }
            if(j==9)
            {
                if(!str.isEmpty())str=QString("%1").arg(str, 8, '0');
            }
            QTableWidgetItem *item = new QTableWidgetItem(str);
            //qDebug()<<i<<j<<str;
            ui->qsoTableWidget->setItem(i, j, item);
        }
    }
    sqlite3_close(db);
}

//Add new QSO
void MainWindow::on_addQSOPushButton_clicked()
{
    qsoDialog.show();
}

void MainWindow::qsoDialogReturned()
{
    refreshQSOTableWidget();
}

//Delete QSO(s)
void MainWindow::on_pushButton_2_clicked()
{
    QList<QTableWidgetSelectionRange>ranges=ui->qsoTableWidget->selectedRanges();
    int count = ranges.count();
    int allcount = 0;
    for(int i=0;i<count;i++)
    {
        allcount += ranges.at(i).bottomRow() - ranges.at(i).topRow() + 1;
    }
    qDebug()<<count<<","<<allcount;
    if(!count)return;

    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Warning);
    msg.setWindowTitle("Delete QSO(s)");

    if(allcount == 1)msg.setText("Do you want to delete this QSO?");
    else msg.setText("Do you want to delete these " + QString::number(allcount) + " QSOs?");
    msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msg.setDefaultButton(QMessageBox::No);
    int rec = msg.exec();

    if (rec == QMessageBox::No)return;

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("QSLManagerSystem.db", &db);
    if(rc)
    {
        qDebug() << "Can't open database: " << zErrMsg;
        exit(0);
    }

    for(int i=0;i<count;i++)
    {
        int topRow=ranges.at(i).topRow();
        int bottomRow=ranges.at(i).bottomRow();
        for(int j=topRow;j<=bottomRow;j++)
        {
            //qDebug()<<"select QSO ID"<<ui->qsoTableWidget->item(j,0)->text().toInt();
            int id = ui->qsoTableWidget->item(j,0)->text().toInt();

            QString cmd1 = "delete from QSO where id = " + QString::number(id);
            qDebug() << cmd1;
            char* ch = cmd1.toUtf8().data();
            rc = sqlite3_exec(db, ch, NULL, 0, &zErrMsg);
            if(rc)
            {
                qDebug() << "Can't delete QSO(s): " << zErrMsg;
                exit(0);
            }
        }
    }

    sqlite3_close(db);
    refreshQSOTableWidget();
}

//Double click any line of tavlewidget to open QSO Detail window
void MainWindow::on_qsoTableWidget_cellDoubleClicked(int row, int column)
{
    //qDebug()<<row<<","<<column;
    detailDialog.show();
    detailDialog.qsoDetailDialogStart(ui->qsoTableWidget->item(row,0)->text().toInt(),
                              ui->qsoTableWidget->item(row,1)->text(),
                              ui->qsoTableWidget->item(row,2)->text(),
                              ui->qsoTableWidget->item(row,3)->text(),
                              ui->qsoTableWidget->item(row,4)->text(),
                              ui->qsoTableWidget->item(row,5)->text(),
                              ui->qsoTableWidget->item(row,6)->text(),
                              ui->qsoTableWidget->item(row,7)->text(),
                              ui->qsoTableWidget->item(row,10)->text(),
                              ui->qsoTableWidget->item(row,8)->text().toInt(),
                              ui->qsoTableWidget->item(row,9)->text().toInt());
}


void MainWindow::on_callsignLineEdit_editingFinished()
{
    refreshQSOTableWidget();
}


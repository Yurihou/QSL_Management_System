#include "qslreceivedialog.h"
#include "ui_qslreceivedialog.h"

#include <QFileDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "sqlite3.h"

QSLReceiveDialog::QSLReceiveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSLReceiveDialog)
{
    ui->setupUi(this);

    ui->recvDateLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    ui->recvDateLineEdit->setText(QDateTime::currentDateTimeUtc().toString("yyyyMMdd"));
}

QSLReceiveDialog::~QSLReceiveDialog()
{
    delete ui;
}

void QSLReceiveDialog::putQSOID(int id, int qslId)
{
    ui->qsoIDLabel->setText(QString("%1").arg(QString::number(id), 8, '0'));
    if(qslId)
    {
        ui->qslIDLabel->setText(QString("%1").arg(QString::number(qslId), 8, '0'));
        QImage image1, image2;// = new QImage;
        QString img1_dir = "./qsl_img/"+QString::number(qslId)+"_1.jpg";
        QString img2_dir = "./qsl_img/"+QString::number(qslId)+"_2.jpg";
        QFile img1_file(img1_dir);
        QFile img2_file(img2_dir);
        if(img1_file.exists())
        {
            ui->dirLabel_1->setText(img1_dir);
            image1.load(img1_dir);
            ui->imageLabel_1->setPixmap(QPixmap::fromImage(image1).scaled(ui->imageLabel_1->size(), Qt::KeepAspectRatio));
        }
        else
        {
            ui->imageLabel_1->clear();
            ui->imageLabel_1->setText("No Image");
        }
        if(img2_file.exists())
        {
            ui->dirLabel_2->setText(img2_dir);
            image2.load(img2_dir);
            ui->imageLabel_2->setPixmap(QPixmap::fromImage(image2).scaled(ui->imageLabel_2->size(), Qt::KeepAspectRatio));
        }
        else
        {
            ui->imageLabel_2->clear();
            ui->imageLabel_2->setText("No Image");
        }

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
        QString cmd2="select qso_date from QSL where id = " + ui->qslIDLabel->text();
        qDebug()<<cmd2;
        if (sqlite3_get_table(db, cmd2.toUtf8().data(), &result, &nrow, &ncolumn, &errmsg) != 0)
        {
            qDebug() << "Can't open table QSO: " << errmsg;
            exit(0);
        }
        QString qslDate(result[1]);
        ui->recvDateLineEdit->setText(qslDate);
        sqlite3_close(db);

    }
    else
    {
        ui->dirLabel_1->setText("No Image");
        ui->dirLabel_2->setText("No Image");
        ui->qslIDLabel->setText("No QSL Received");
        ui->imageLabel_1->clear();
        ui->imageLabel_1->setText("No Image");
        ui->imageLabel_2->clear();
        ui->imageLabel_2->setText("No Image");
    }
}

//Open/Import Front Image(jpg)
void QSLReceiveDialog::on_pushButton_clicked()
{
    QString str =  QFileDialog::getOpenFileName(this, "Select QSL image", "./qsl_img/", "Images Files [*.png *.jpg] (*.png *.jpg)");
    if(str.isNull())
    {
        ui->imageLabel_1->clear();
        ui->imageLabel_1->setText("No Image");
        return;
    }
    ui->dirLabel_1->setText(str);
    QImage image;// = new QImage;
    image.load(ui->dirLabel_1->text());
    ui->imageLabel_1->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel_1->size(), Qt::KeepAspectRatio));
}

//Open/Import Backward Image
void QSLReceiveDialog::on_pushButton_5_clicked()
{
    QString str =  QFileDialog::getOpenFileName(this, "Select QSL image", "./qsl_img/", "Images Files [*.jpg] (*.jpg)");
    if(str.isNull())
    {
        ui->imageLabel_1->clear();
        ui->imageLabel_1->setText("No Image");
        return;
    }
    ui->dirLabel_2->setText(str);
    QImage image;// = new QImage;
    image.load(ui->dirLabel_2->text());
    ui->imageLabel_2->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel_2->size(), Qt::KeepAspectRatio));
}

//Import image, Log QSL and Update QSO table
void QSLReceiveDialog::on_pushButton_4_clicked()
{
    QString qslId;

    bool isQsl=false;
    ui->qslIDLabel->text().toInt(&isQsl);

    QString cmd1;
    QTextStream cmdLine1(&cmd1);
    cmdLine1 << "update QSO set ";

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("QSLManagerSystem.db", &db);
    if(rc)
    {
        qDebug() << "Can't open database: " << zErrMsg;
        exit(0);
    }

    if(isQsl)
    {
        qslId=QString::number(ui->qslIDLabel->text().toInt());
        QString cmd1="update QSL set qso_date = \"" + ui->recvDateLineEdit->text() + "\" where id == " + qslId;
        rc = sqlite3_exec(db, cmd1.toUtf8().data(), NULL, 0, &zErrMsg);
        if(rc)
        {
            qDebug() << "Can't update QSL info: " << zErrMsg;
            exit(0);
        }
    }
    else
    {
        QString cmd1="insert into QSL (qso_id, qso_date) values (" + ui->qsoIDLabel->text() + ", \"" + ui->recvDateLineEdit->text() + "\")";
        qDebug()<<cmd1;
        rc = sqlite3_exec(db, cmd1.toUtf8().data(), NULL, 0, &zErrMsg);
        if(rc)
        {
            qDebug() << "Can't insert QSL info: " << zErrMsg;
            exit(0);
        }

        char **result, *errmsg;
        int nrow, ncolumn;
        QString cmd2="select id from QSL where qso_id = " + ui->qsoIDLabel->text();
        qDebug()<<cmd2;
        if (sqlite3_get_table(db, cmd2.toUtf8().data(), &result, &nrow, &ncolumn, &errmsg) != 0)
        {
            qDebug() << "Can't open table QSO: " << errmsg;
            exit(0);
        }
        qslId = QString(result[1]);
    }

    QString cmd11="update QSO set qsl_recv = \"" + qslId + "\" where id == " + ui->qsoIDLabel->text();
    qDebug()<<cmd11;
    rc = sqlite3_exec(db, cmd11.toUtf8().data(), NULL, 0, &zErrMsg);
    if(rc)
    {
        qDebug() << "Can't update QSO info: " << zErrMsg;
        exit(0);
    }

    sqlite3_close(db);


    QFile img1_file(ui->dirLabel_1->text());
    if(img1_file.exists())
    {
        //QFileInfo imf(ui->dirLabel_1->text());
        //QString suf=imf.suffix();
        qDebug()<<qslId;
        QFile im(ui->dirLabel_1->text());
        QString tgt_img="./qsl_img/"+qslId+"_1.jpg";
        QFile tg(tgt_img);
        if(ui->dirLabel_1->text()!=tgt_img && tg.exists())
        {
            tg.remove(tgt_img);
            im.copy(tgt_img);
        }
        else if(!tg.exists())im.copy(tgt_img);
    }

    QFile img2_file(ui->dirLabel_2->text());
    if(img2_file.exists())
    {
        //QFileInfo imf(ui->dirLabel_2->text());
        //QString suf=imf.suffix();
        //qDebug()<<suf;
        QFile im(ui->dirLabel_2->text());
        QString tgt_img="./qsl_img/"+qslId+"_2.jpg";
        QFile tg(tgt_img);
        if(ui->dirLabel_2->text()!=tgt_img && tg.exists())
        {
            tg.remove(tgt_img);
            im.copy(tgt_img);
        }
        else if(!tg.exists())im.copy(tgt_img);
    }

    emit qslModified1(qslId);
}

//Close window
void QSLReceiveDialog::on_pushButton_2_clicked()
{
    this->close();
}


void QSLReceiveDialog::on_pushButton_4_clicked(bool checked)
{

}


void QSLReceiveDialog::on_pushButton_clicked(bool checked)
{

}


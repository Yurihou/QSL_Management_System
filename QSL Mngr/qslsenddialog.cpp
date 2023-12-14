#include "qslsenddialog.h"
#include "ui_qslsenddialog.h"

#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "sqlite3.h"

QSLSendDialog::QSLSendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSLSendDialog)
{
    ui->setupUi(this);

    ui->dateLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    ui->dateLineEdit->setText(QDateTime::currentDateTimeUtc().toString("yyyyMMdd"));
}

QSLSendDialog::~QSLSendDialog()
{
    delete ui;
}

void QSLSendDialog::putQSOID(int id, int qslId)
{
    ui->qsoIDLabel->setText(QString("%1").arg(QString::number(id), 8, '0'));
    if(qslId)ui->dateLineEdit->setText(QString("%1").arg(QString::number(qslId), 8, '0'));
}

//Log QSL send time and update QSO table
void QSLSendDialog::on_pushButton_4_clicked()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("QSLManagerSystem.db", &db);
    if(rc)
    {
        qDebug() << "Can't open database: " << zErrMsg;
        exit(0);
    }

    QString cmd11="update QSO set qsl_send = \"" + ui->dateLineEdit->text() + "\" where id == " + ui->qsoIDLabel->text();
    qDebug()<<cmd11;
    rc = sqlite3_exec(db, cmd11.toUtf8().data(), NULL, 0, &zErrMsg);
    if(rc)
    {
        qDebug() << "Can't update QSO info: " << zErrMsg;
        exit(0);
    }

    sqlite3_close(db);
    emit qslModified2();
}

//Close window
void QSLSendDialog::on_pushButton_5_clicked()
{
    this->close();
}


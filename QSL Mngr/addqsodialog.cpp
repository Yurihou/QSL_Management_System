#include "addqsodialog.h"
#include "ui_addqsodialog.h"

#include <QMessageBox>
#include <QDateTime>
#include <QRegularExpression>
#include "sqlite3.h"

AddQSODialog::AddQSODialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddQSODialog)
{
    ui->setupUi(this);
    ui->qsoDateLineEdit->setText(QDateTime::currentDateTimeUtc().toString("yyyyMMdd"));
    ui->qsoDateLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    ui->qsoUTCLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
}

AddQSODialog::~AddQSODialog()
{
    delete ui;
}

void AddQSODialog::accept()
{
    qDebug() << "ok";
    int cmdsec = 0;
    QString cmd1, cmd2;
    QTextStream cmdLine1(&cmd1);
    QTextStream cmdLine2(&cmd2);
    cmdLine1 << "insert into QSO(";
    cmdLine2 << ")values(";

    if(!ui->qsoDateLineEdit->text().isEmpty())
    {
        cmdLine1 << "date";
        cmdLine2 << "\"" << ui->qsoDateLineEdit->text() << "\"";
        cmdsec++;
    }
    else
    {
        QMessageBox aaa;
        aaa.setText("QSO Date cannot be left blank.");
        aaa.exec();
        return;
    }

    if(!ui->qsoUTCLineEdit->text().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
            cmdLine2 << ",";
        }
        cmdLine1 << "utc";
        cmdLine2 << "\"" << ui->qsoUTCLineEdit->text() << "\"";
        cmdsec++;
    }

    if(!ui->callsignLineEdit->text().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
            cmdLine2 << ",";
        }
        cmdLine1 << "call";
        cmdLine2 << "\"" << ui->callsignLineEdit->text() << "\"";
        cmdsec++;
    }
    else
    {
        QMessageBox aaa;
        aaa.setText("Callsign cannot be left blank.");
        aaa.exec();
        return;
    }

    if(!ui->freqLineEdit->text().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
            cmdLine2 << ",";
        }
        cmdLine1 << "freq";
        cmdLine2 << "\"" << ui->freqLineEdit->text() << "\"";
        cmdsec++;
    }

    if(!ui->comboBox->currentText().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
            cmdLine2 << ",";
        }
        cmdLine1 << "mode";
        cmdLine2 << "\"" << ui->comboBox->currentText() << "\"";
        cmdsec++;
    }
    else
    {
        QMessageBox aaa;
        aaa.setText("Mode cannot be left blank.");
        aaa.exec();
        return;
    }

    if(!ui->rstSendLineEdit->text().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
            cmdLine2 << ",";
        }
        cmdLine1 << "rst_send";
        cmdLine2 << "\"" << ui->rstSendLineEdit->text() << "\"";
        cmdsec++;
    }

    if(!ui->rstRecvLineEdit->text().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
            cmdLine2 << ",";
        }
        cmdLine1 << "rst_recv";
        cmdLine2 << "\"" << ui->rstRecvLineEdit->text() << "\"";
        cmdsec++;
    }

    if(!ui->noteTextEdit->toPlainText().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
            cmdLine2 << ",";
        }
        cmdLine1 << "note";
        cmdLine2 << "\"" << ui->noteTextEdit->toPlainText() << "\"";
        cmdsec++;
    }

    cmdLine1 << cmd2 << ")";
    qDebug() << cmd1;

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("QSLManagerSystem.db", &db);
    if(rc)
    {
        qDebug() << "Can't open database: " << zErrMsg;
        exit(0);
    }

    char* ch = cmd1.toUtf8().data();
    rc = sqlite3_exec(db, ch, NULL, 0, &zErrMsg);
    if(rc)
    {
        qDebug() << "Can't insert QSO: " << zErrMsg;
        exit(0);
    }

    sqlite3_close(db);

    emit qsoInserted();

    done(Accepted);               /* 窗口关闭前发出Accepted信号，不调用该函数窗口不退出 */
}

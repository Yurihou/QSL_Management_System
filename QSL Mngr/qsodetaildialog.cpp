#include "qsodetaildialog.h"
#include "ui_qsodetaildialog.h"
#include "sqlite3.h"

#include <QMessageBox>
#include <QFile>

QSODetailDialog::QSODetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSODetailDialog)
{
    ui->setupUi(this);

    connect(&receiveDialog,SIGNAL(qslModified1(QString)),this,SLOT(updateQSL(QString)));
    connect(&sendDialog,SIGNAL(qslModified2()),this,SLOT(updateQSL2()));

    ui->qsoDateLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    ui->qsoUTCLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
}

QSODetailDialog::~QSODetailDialog()
{
    delete ui;
}

//Pass QSO information
void QSODetailDialog::qsoDetailDialogStart(int id, QString date, QString utc, QString call, QString freq, QString mode, QString rst_send, QString rst_recv, QString note, int qsl_send, int qsl_recv)
{
    ui->qsoIDLabel->setText(QString("%1").arg(QString::number(id), 8, '0'));
    ui->qsoDateLineEdit->setText(date);
    ui->qsoUTCLineEdit->setText(utc);
    ui->callsignLineEdit->setText(call);
    ui->freqLineEdit->setText(freq);
    ui->comboBox->setCurrentText(mode);
    ui->rstSendLineEdit->setText(rst_send);
    ui->rstRecvLineEdit->setText(rst_recv);
    ui->noteTextEdit->setText(note);
    //this->qsl_send = qsl_send;
    qDebug()<<qsl_recv;
    if(qsl_send==0)ui->qslSendLabel->setText("Not yet");
    else ui->qslSendLabel->setText(QString("%1").arg(QString::number(qsl_send), 8, '0'));
    //this->qsl_recv = qsl_recv;
    if(qsl_recv==0)
    {
        ui->qslRecvLabel->setText("Not yet");
        ui->qslImageLabel->clear();
        ui->qslImageLabel->setText("No QSL Image");
    }
    else
    {
        QImage image1;// = new QImage;
        QString img1_dir = "./qsl_img/"+QString::number(qsl_recv)+"_1.jpg";
        QFile img1_file(img1_dir);
        if(img1_file.exists())
        {
            image1.load(img1_dir);
            ui->qslImageLabel->setPixmap(QPixmap::fromImage(image1).scaled(ui->qslImageLabel->size(), Qt::KeepAspectRatio));
        }
        ui->qslRecvLabel->setText(QString("%1").arg(QString::number(qsl_recv), 8, '0'));
    }
    //qDebug()<<qsl_send<<","<<qsl_recv;

    this->setWindowTitle("QSO Details");
    qsoChanged = 0;
}

//Rewrite closeevent to block close event without save/modification
//Save? Discard? Cancel?
void QSODetailDialog::closeEvent(QCloseEvent * event)
{
    if (qsoChanged)
    {
        QMessageBox msgBox;
        msgBox.setText("The QSO has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Save)
        {
            updateQSO();
        }
        else if(ret == QMessageBox::Cancel)event->ignore();
    }
}

//Update receive QSL
void QSODetailDialog::updateQSL(QString qslId)
{
    ui->qslRecvLabel->setText(QString("%1").arg(qslId, 8, '0'));
    if(qslId.toInt()==0)ui->qslRecvLabel->setText("Not yet");
    else
    {
        QImage image1;// = new QImage;
        QString img1_dir = "./qsl_img/"+QString::number(qslId.toInt())+"_1.jpg";
        QFile img1_file(img1_dir);
        if(img1_file.exists())
        {
            image1.load(img1_dir);
            ui->qslImageLabel->setPixmap(QPixmap::fromImage(image1).scaled(ui->qslImageLabel->size(), Qt::KeepAspectRatio));
        }
        ui->qslRecvLabel->setText(QString("%1").arg(QString::number(qslId.toInt()), 8, '0'));
    }
    //updateQSO();
    emit qsoModifiedOrDeleted();
}

//Update send QSL
void QSODetailDialog::updateQSL2()
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

    char **result, *errmsg;
    int nrow, ncolumn;

    QString cmd = "select qsl_send from QSO where id == " + ui->qsoIDLabel->text();
    //qDebug() << cmd;
    if (sqlite3_get_table(db, cmd.toUtf8().data(), &result, &nrow, &ncolumn, &errmsg) != 0)
    {
        qDebug() << "Can't open table QSO: " << errmsg;
        exit(0);
    }

    QString str(result[1]);
    if(str.toInt()==0)ui->qslSendLabel->setText("Not yet");
    else ui->qslSendLabel->setText(QString("%1").arg(str, 8, '0'));

    sqlite3_close(db);
    emit qsoModifiedOrDeleted();
}

//Update QSO table
void QSODetailDialog::updateQSO()
{
    qDebug() << "ok";
    int cmdsec = 0;
    QString cmd1;
    QTextStream cmdLine1(&cmd1);
    cmdLine1 << "update QSO set ";

    if(!ui->qsoDateLineEdit->text().isEmpty())
    {
        cmdLine1 << "date = ";
        cmdLine1 << "\"" << ui->qsoDateLineEdit->text() << "\"";
        cmdsec++;
    }
    else
    {
        QMessageBox aaa;
        aaa.setText("QSO Date cannot be left blank.");
        aaa.exec();
        return;
    }

    if(cmdsec)
    {
        cmdLine1 << ",";
    }
    cmdLine1 << "utc = ";
    cmdLine1 << "\"" << ui->qsoUTCLineEdit->text() << "\"";
    cmdsec++;


    if(!ui->callsignLineEdit->text().isEmpty())
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
        }
        cmdLine1 << "call = ";
        cmdLine1 << "\"" << ui->callsignLineEdit->text() << "\"";
        cmdsec++;
    }
    else
    {
        QMessageBox aaa;
        aaa.setText("Callsign cannot be left blank.");
        aaa.exec();
        return;
    }

    if(cmdsec)
    {
        cmdLine1 << ",";
    }
    cmdLine1 << "freq = ";
    cmdLine1 << "\"" << ui->freqLineEdit->text() << "\"";
    cmdsec++;

    if(1)
    {
        if(cmdsec)
        {
            cmdLine1 << ",";
        }
        cmdLine1 << "mode = ";
        cmdLine1 << "\"" << ui->comboBox->currentText() << "\"";
        cmdsec++;
    }
    else
    {
        QMessageBox aaa;
        aaa.setText("Mode cannot be left blank.");
        aaa.exec();
        return;
    }

    if(cmdsec)
    {
        cmdLine1 << ",";
    }
    cmdLine1 << "rst_send = ";
    cmdLine1 << "\"" << ui->rstSendLineEdit->text() << "\"";
    cmdsec++;

    if(cmdsec)
    {
        cmdLine1 << ",";
    }
    cmdLine1 << "rst_recv = ";
    cmdLine1 << "\"" << ui->rstRecvLineEdit->text() << "\"";
    cmdsec++;

    if(cmdsec)
    {
        cmdLine1 << ",";
    }
    cmdLine1 << "note = ";
    cmdLine1 << "\"" << ui->noteTextEdit->toPlainText() << "\"";
    cmdsec++;

    cmdLine1 << " where id == " << QString::number(ui->qsoIDLabel->text().toInt());
    //qDebug() << cmd1;

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
        qDebug() << "Can't update QSO: " << zErrMsg;
        exit(0);
    }

    sqlite3_close(db);
    emit qsoModifiedOrDeleted();

    this->setWindowTitle("QSO Details");
    qsoChanged = 0;
}

//If any textbox was modified, make a block before close this window
void QSODetailDialog::changeQSO()
{
    this->setWindowTitle("QSO Details*");
    qsoChanged = 1;
}

void QSODetailDialog::on_qsoDateLineEdit_editingFinished()
{
    changeQSO();
}


void QSODetailDialog::on_qsoUTCLineEdit_editingFinished()
{
    changeQSO();
}


void QSODetailDialog::on_callsignLineEdit_editingFinished()
{
    changeQSO();
}


void QSODetailDialog::on_freqLineEdit_editingFinished()
{
    changeQSO();
}


void QSODetailDialog::on_comboBox_currentTextChanged(const QString &arg1)
{
    changeQSO();
}


void QSODetailDialog::on_comboBox_editTextChanged(const QString &arg1)
{
    changeQSO();
}


void QSODetailDialog::on_rstSendLineEdit_editingFinished()
{
    changeQSO();
}


void QSODetailDialog::on_rstRecvLineEdit_editingFinished()
{
    changeQSO();
}


void QSODetailDialog::on_noteTextEdit_textChanged()
{
    changeQSO();
}

//Close Window
void QSODetailDialog::on_pushButton_5_clicked()
{
    this->close();
}

//Save/Modify QSO button
void QSODetailDialog::on_pushButton_3_clicked()
{
    updateQSO();
}

//Delete this QSO
void QSODetailDialog::on_pushButton_4_clicked()
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Warning);
    msg.setWindowTitle("Delete QSO(s)");

    msg.setText("Do you want to delete this QSO?");
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

    QString cmd1 = "delete from QSO where id = ";
    cmd1 += QString::number(ui->qsoIDLabel->text().toInt());

    char* ch = cmd1.toUtf8().data();
    rc = sqlite3_exec(db, ch, NULL, 0, &zErrMsg);
    if(rc)
    {
        qDebug() << "Can't delete QSO: " << zErrMsg;
        exit(0);
    }

    sqlite3_close(db);

    emit qsoModifiedOrDeleted();

    qsoChanged = 0;
    this->close();
}

//Open receive QSL window
void QSODetailDialog::on_pushButton_2_clicked()
{
    int aaa = 0;
    aaa = ui->qslRecvLabel->text().toInt();
    receiveDialog.show();
    receiveDialog.putQSOID(ui->qsoIDLabel->text().toInt(), aaa);
}

//Qpen send QSL window
void QSODetailDialog::on_pushButton_clicked()
{
    int aaa = 0;
    aaa = ui->qslSendLabel->text().toInt();
    sendDialog.show();
    sendDialog.putQSOID(ui->qsoIDLabel->text().toInt(), aaa);
}


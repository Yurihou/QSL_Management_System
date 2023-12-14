#ifndef QSODETAILDIALOG_H
#define QSODETAILDIALOG_H

#include <QDialog>
#include <QCloseEvent>

#include "qslreceivedialog.h"
#include "qslsenddialog.h"

namespace Ui {
class QSODetailDialog;
}

class QSODetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSODetailDialog(QWidget *parent = nullptr);
    ~QSODetailDialog();
    void qsoDetailDialogStart(int id, QString date, QString utc, QString call, QString freq, QString mode, QString rst_send, QString rst_recv, QString note, int qsl_send, int qsl_recv);

protected :
    void closeEvent(QCloseEvent *event);

private slots:
    void on_qsoDateLineEdit_editingFinished();

    void on_qsoUTCLineEdit_editingFinished();

    void on_callsignLineEdit_editingFinished();

    void on_freqLineEdit_editingFinished();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_editTextChanged(const QString &arg1);

    void on_rstSendLineEdit_editingFinished();

    void on_rstRecvLineEdit_editingFinished();

    void on_noteTextEdit_textChanged();

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    //void qslModified();
    void updateQSO();
    void updateQSL(QString qslId);
    void updateQSL2();

    void on_pushButton_clicked();

private:
    Ui::QSODetailDialog *ui;
    //int qsl_send = 0;
    //int qsl_recv = 0;
    int qsoChanged = 0;


    void changeQSO();

    QSLReceiveDialog receiveDialog;
    QSLSendDialog sendDialog;

signals:
    void qsoModifiedOrDeleted();
};

#endif // QSODETAILDIALOG_H

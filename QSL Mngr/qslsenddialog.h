#ifndef QSLSENDDIALOG_H
#define QSLSENDDIALOG_H

#include <QDialog>

namespace Ui {
class QSLSendDialog;
}

class QSLSendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSLSendDialog(QWidget *parent = nullptr);
    ~QSLSendDialog();

    void putQSOID(int id, int qslId);

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::QSLSendDialog *ui;

signals:
    void qslModified2();
};

#endif // QSLSENDDIALOG_H

#ifndef QSLRECEIVEDIALOG_H
#define QSLRECEIVEDIALOG_H

#include <QDialog>

namespace Ui {
class QSLReceiveDialog;
}

class QSLReceiveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSLReceiveDialog(QWidget *parent = nullptr);
    ~QSLReceiveDialog();

    void putQSOID(int id, int qslId);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked(bool checked);

    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked(bool checked);


private:
    Ui::QSLReceiveDialog *ui;
    //QString imagefile = "";

signals:
    void qslModified1(QString qslId);

};

#endif // QSLRECEIVEDIALOG_H

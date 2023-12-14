#ifndef ADDQSODIALOG_H
#define ADDQSODIALOG_H

#include <QDialog>

namespace Ui {
class AddQSODialog;
}

class AddQSODialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddQSODialog(QWidget *parent = nullptr);
    ~AddQSODialog();

private slots:
    void accept() override;
    //void reject() override;

private:
    Ui::AddQSODialog *ui;

signals:
    void qsoInserted();
};

#endif // ADDQSODIALOG_H

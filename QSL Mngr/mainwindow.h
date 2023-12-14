#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addqsodialog.h"
#include "qsodetaildialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addQSOPushButton_clicked();
    void qsoDialogReturned();
    void on_pushButton_2_clicked();
    void on_qsoTableWidget_cellDoubleClicked(int row, int column);

    void on_callsignLineEdit_editingFinished();

private:
    Ui::MainWindow *ui;
    AddQSODialog qsoDialog;
    QSODetailDialog detailDialog;

    void refreshQSOTableWidget();
};
#endif // MAINWINDOW_H

#include "mainwindow.h"

#include "sqlite3.h"

#include <QApplication>
#include <QDir>

void sqlite3_init()
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

    rc = sqlite3_exec(db, "select * from QSO", NULL, 0, &zErrMsg);
    if(rc)
    {
        char sqlcmd[]="create table QSO(\
id integer primary key autoincrement,\
date text no null,\
utc text,\
call text no null,\
freq text,\
mode text no null,\
rst_send text,\
rst_recv text,\
qsl_send integer,\
qsl_recv integer,\
note text)";
        rc = sqlite3_exec(db, sqlcmd, NULL, 0, &zErrMsg);
        if(rc)
        {
            qDebug() << "Can't form table QSO: " << zErrMsg;
            exit(0);
        }
    }

    rc = sqlite3_exec(db, "select * from QTH", NULL, 0, &zErrMsg);
    if(rc)
    {
        char sqlcmd[]="create table QTH(\
callsign text no null,\
address text,\
zipcode text)";
        rc = sqlite3_exec(db, sqlcmd, NULL, 0, &zErrMsg);
        if(rc)
        {
            qDebug() << "Can't form table QTH: " << zErrMsg;
            exit(0);
        }
    }

    rc = sqlite3_exec(db, "select * from QSL", NULL, 0, &zErrMsg);
    if(rc)
    {
        char sqlcmd[]="create table QSL(\
id integer primary key autoincrement,\
qso_id integer no null,\
qso_date text)";
        rc = sqlite3_exec(db, sqlcmd, NULL, 0, &zErrMsg);
        if(rc)
        {
            qDebug() << "Can't form table QSL: " << zErrMsg;
            exit(0);
        }
    }
    sqlite3_close(db);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sqlite3_init();

    QDir dir(QDir::currentPath());    //初始化dir为当前目录
    QString folder_name("qsl_img");
    //假设当前目录下有一个test文件夹
    if(!dir.exists(folder_name))    //如果Images文件夹不存在
    {
        dir.mkdir(folder_name);    //创建文件夹（名为Images）
        qDebug()<<QString("文件夹%1创建成功！").arg(folder_name);
    }
    else
    {
        qDebug()<<QString("文件夹%1已存在！").arg(folder_name);
    }

    MainWindow w;
    w.show();
    return a.exec();
}

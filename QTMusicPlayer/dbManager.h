#ifndef DBMANAGER_H
#define DBMANAGER_H


#include <QObject>
#include <QSqlQuery>
#include <QMutex>
#include <QMap>

//数据库操作类
class dbManager : public QObject
{
    Q_OBJECT
public:
    //以connName连接对数据库dbFile执行sql语句，该sql语句无返回值，
    static bool excute(QString sql, QString connName, QString dbFile = "QTMusicPlayer.db");
    //以connName连接对数据库dbFile执行sql语句，该sql语句有返回值
    static QSqlQuery* select(QString sql, QString connName, QString dbFile = "QTMusicPlayer.db");
    //删除db文件dbFile的connName连接以及对应的锁
    static void destoryConn(QString connName,QString dbFile = "QTMusicPlayer.db");
    //删除db文件dbFile的所有连接和锁
    static void destoryOneDBConn(QString dbFile = "QTMusicPlayer.db");
    //删除所有db文件的所有连接和锁
    static void destoryAllDBConn();

    //获取对于dbFile文件的数据库连接connName
    //有则返回，没有则创建再返回
    static QSqlDatabase getSqlDataBase(QString connName, QString dbFile);
    //同步锁，外层key是db文件名，内层key是该db文件的数据库连接名
    static QMap<QString, QMap<QString,QMutex*>> mutexMap;
};


#endif // DBMANAGER_H

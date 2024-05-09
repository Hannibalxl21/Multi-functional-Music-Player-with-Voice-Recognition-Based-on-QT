#include "dbManager.h"

QMap<QString, QMap<QString, QMutex*>> dbManager::mutexMap;

bool dbManager::excute(QString sql, QString connName, QString dbFile)
{
    QSqlDatabase db = getSqlDataBase(connName, dbFile);

    mutexMap[dbFile][connName]->lock();
    if(!db.isOpen())
        db.open();

    QSqlQuery sqlQuery(db);
    bool flag = sqlQuery.exec(sql);

    mutexMap[dbFile][connName]->unlock();
    return flag;
}

QSqlQuery* dbManager::select(QString sql, QString connName, QString dbFile)
{
    QSqlDatabase db = getSqlDataBase(connName, dbFile);

    mutexMap[dbFile][connName]->lock();
    if(!db.isOpen())
        db.open();

    QSqlQuery *sqlQuery = new QSqlQuery(db);
    sqlQuery->exec(sql);

    mutexMap[dbFile][connName]->unlock();
    return sqlQuery;
}

void dbManager::destoryConn(QString connName, QString dbFile)
{
    if (!QSqlDatabase::contains(dbFile + connName))
        return ;

    QSqlDatabase db = QSqlDatabase::database(dbFile + connName);
    if (db.isOpen())
        db.close();

    QSqlDatabase::removeDatabase(dbFile + connName);

    delete mutexMap[dbFile][connName];

    mutexMap[dbFile].remove(connName);
    if(mutexMap[dbFile].keys().length() == 0)
        mutexMap.remove(dbFile);
}

void dbManager::destoryOneDBConn(QString dbFile)
{
    foreach (QString i, mutexMap[dbFile].keys())
    {
        destoryConn(i,dbFile);
    }
}

void dbManager::destoryAllDBConn()
{
    foreach (QString i,mutexMap.keys())
    {
        destoryOneDBConn(i);
    }
}

QSqlDatabase dbManager::getSqlDataBase(QString connName, QString dbFile)
{
    if (!QSqlDatabase::contains(dbFile + connName))
    {
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", dbFile + connName);
        database .setDatabaseName(dbFile);
        if (!mutexMap.contains(dbFile))
        {
            QMap<QString, QMutex*> tMap;
            mutexMap.insert(dbFile, tMap);
        }
        QMutex *mutex = new QMutex();
        mutexMap[dbFile].insert(connName, mutex);
    }
    return QSqlDatabase::database(dbFile + connName);
}


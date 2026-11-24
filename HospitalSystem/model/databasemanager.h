#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

// 数据库访问的唯一入口：负责连接、建表和初始数据。
// 采用单例，保证整个程序只持有一个 QSqlDatabase 连接。
class DatabaseManager
{
public:
    static DatabaseManager &instance();

    bool open(const QString &filePath, QString *error = nullptr);
    QSqlDatabase database() const;

    bool createSchema(QString *error = nullptr);
    bool seedDefaultData(QString *error = nullptr);

    static QString defaultDatabasePath();
    static QString passwordHash(const QString &plain);

private:
    DatabaseManager() = default;
    QSqlDatabase m_database;
};

#endif // DATABASEMANAGER_H

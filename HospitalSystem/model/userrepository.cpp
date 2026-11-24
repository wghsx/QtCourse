#include "userrepository.h"
#include "databasemanager.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

bool UserRepository::authenticate(const QString &username, const QString &password,
                                  QString *userId, QString *fullName, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(
        "SELECT ID, FULLNAME FROM \"User\" WHERE USERNAME = ? AND PASSWORD = ?"));
    query.addBindValue(username);
    query.addBindValue(DatabaseManager::passwordHash(password));

    if (!query.exec()) {
        if (error)
            *error = query.lastError().text();
        return false;
    }
    if (!query.next())
        return false;

    if (userId)
        *userId = query.value(0).toString();
    if (fullName)
        *fullName = query.value(1).toString();
    return true;
}

bool UserRepository::exists(const QString &username, bool *exists, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM \"User\" WHERE USERNAME = ?"));
    query.addBindValue(username);
    if (!query.exec()) {
        if (error)
            *error = query.lastError().text();
        return false;
    }
    *exists = query.next() && query.value(0).toInt() > 0;
    return true;
}

bool UserRepository::create(const QString &fullName, const QString &username,
                            const QString &password, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(
        "INSERT INTO \"User\" (ID, FULLNAME, USERNAME, PASSWORD) VALUES (?, ?, ?, ?)"));
    query.addBindValue(QStringLiteral("U%1").arg(
        QDateTime::currentDateTime().toString(QStringLiteral("yyMMddhhmmss"))));
    query.addBindValue(fullName);
    query.addBindValue(username);
    query.addBindValue(DatabaseManager::passwordHash(password));

    if (!query.exec()) {
        if (error) {
            if (query.lastError().text().contains(QStringLiteral("UNIQUE")))
                *error = QStringLiteral("该用户名已被占用，请更换后重试。");
            else
                *error = query.lastError().text();
        }
        return false;
    }
    return true;
}

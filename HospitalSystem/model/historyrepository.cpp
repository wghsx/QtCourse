#include "historyrepository.h"
#include "databasemanager.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

bool HistoryRepository::record(const QString &userId, const QString &event)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(
        "INSERT INTO History (USER_ID, EVENT, TIMESTAMP) VALUES (?, ?, ?)"));
    query.addBindValue(userId);
    query.addBindValue(event);
    query.addBindValue(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    return query.exec();
}

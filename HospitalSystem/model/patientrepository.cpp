#include "patientrepository.h"
#include "databasemanager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {

// 把 SQLite 的约束错误翻译成用户能看懂的提示。
QString describeError(const QSqlError &error)
{
    const QString text = error.text();
    if (text.contains(QStringLiteral("UNIQUE constraint failed: Patient.ID_CARD")))
        return QStringLiteral("身份证号已存在，请核对后重新录入。");
    if (text.contains(QStringLiteral("UNIQUE constraint failed: Patient.ID")))
        return QStringLiteral("病历号已存在，请重新分配。");
    if (text.contains(QStringLiteral("NOT NULL constraint failed")))
        return QStringLiteral("存在必填字段为空，记录未写入数据库。");
    return text;
}

} // namespace

bool PatientRepository::insert(const PatientRecord &record, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(
        "INSERT INTO Patient (ID, ID_CARD, NAME, SEX, DOB, HEIGHT, WEIGHT,"
        " MOBILEPHONE, AGE, CREATEDTIMESTAMP)"
        " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
    query.addBindValue(record.id);
    query.addBindValue(record.idCard);
    query.addBindValue(record.name);
    query.addBindValue(record.sex);
    query.addBindValue(record.dob);
    query.addBindValue(record.height);
    query.addBindValue(record.weight);
    query.addBindValue(record.mobile);
    query.addBindValue(record.age);
    query.addBindValue(record.createdTimestamp);

    if (!query.exec()) {
        if (error)
            *error = describeError(query.lastError());
        return false;
    }
    return true;
}

bool PatientRepository::update(const PatientRecord &record, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(
        "UPDATE Patient SET ID_CARD = ?, NAME = ?, SEX = ?, DOB = ?, HEIGHT = ?,"
        " WEIGHT = ?, MOBILEPHONE = ?, AGE = ? WHERE ID = ?"));
    query.addBindValue(record.idCard);
    query.addBindValue(record.name);
    query.addBindValue(record.sex);
    query.addBindValue(record.dob);
    query.addBindValue(record.height);
    query.addBindValue(record.weight);
    query.addBindValue(record.mobile);
    query.addBindValue(record.age);
    query.addBindValue(record.id);

    if (!query.exec()) {
        if (error)
            *error = describeError(query.lastError());
        return false;
    }
    if (query.numRowsAffected() == 0) {
        if (error)
            *error = QStringLiteral("记录不存在，可能已被其他用户删除。");
        return false;
    }
    return true;
}

bool PatientRepository::remove(const QString &id, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral("DELETE FROM Patient WHERE ID = ?"));
    query.addBindValue(id);
    if (!query.exec()) {
        if (error)
            *error = describeError(query.lastError());
        return false;
    }
    if (query.numRowsAffected() == 0) {
        if (error)
            *error = QStringLiteral("记录不存在，删除未生效。");
        return false;
    }
    return true;
}

int PatientRepository::count(QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(QStringLiteral("SELECT COUNT(*) FROM Patient"))) {
        if (error)
            *error = query.lastError().text();
        return -1;
    }
    if (query.next())
        return query.value(0).toInt();
    return 0;
}

bool PatientRepository::idCardExists(const QString &idCard, const QString &excludeId,
                                     bool *exists, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    // excludeId 为空时不能绑定空 QString：SQLite 会把它当成 NULL，
    // 而 "ID <> NULL" 永远不成立，预检查就会漏判重复身份证号。
    if (excludeId.isEmpty()) {
        query.prepare(QStringLiteral("SELECT COUNT(*) FROM Patient WHERE ID_CARD = ?"));
        query.addBindValue(idCard);
    } else {
        query.prepare(QStringLiteral(
            "SELECT COUNT(*) FROM Patient WHERE ID_CARD = ? AND ID <> ?"));
        query.addBindValue(idCard);
        query.addBindValue(excludeId);
    }
    if (!query.exec()) {
        if (error)
            *error = query.lastError().text();
        return false;
    }
    if (query.next()) {
        *exists = query.value(0).toInt() > 0;
        return true;
    }
    *exists = false;
    return true;
}

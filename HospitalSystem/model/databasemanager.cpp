#include "databasemanager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {

const char *kConnectionName = "hospital";

// 统一执行一条 SQL，失败时把 SQLite 的错误原样带出去。
bool runStatement(QSqlDatabase database, const QString &sql, QString *error)
{
    QSqlQuery query(database);
    if (query.exec(sql))
        return true;
    if (error)
        *error = query.lastError().text();
    return false;
}

} // namespace

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

bool DatabaseManager::open(const QString &filePath, QString *error)
{
    if (m_database.isValid())
        m_database.close();

    if (QSqlDatabase::contains(QLatin1String(kConnectionName)))
        m_database = QSqlDatabase::database(QLatin1String(kConnectionName), false);
    else
        m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),
                                               QLatin1String(kConnectionName));

    m_database.setDatabaseName(filePath);
    if (!m_database.open()) {
        if (error)
            *error = m_database.lastError().text();
        return false;
    }

    QSqlQuery pragma(m_database);
    pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON"));
    return true;
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

bool DatabaseManager::createSchema(QString *error)
{
    // 字段长度按参考设计收紧：病历号 32、身份证 18、姓名 20 等。
    const QStringList statements = {
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Department ("
            " ID TEXT PRIMARY KEY NOT NULL,"
            " NAME TEXT NOT NULL)"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Doctor ("
            " ID TEXT PRIMARY KEY NOT NULL,"
            " EMPLOYEENO TEXT NOT NULL UNIQUE,"
            " NAME TEXT NOT NULL,"
            " DEPARTMENT_ID TEXT NOT NULL,"
            " FOREIGN KEY (DEPARTMENT_ID) REFERENCES Department(ID))"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Patient ("
            " ID TEXT PRIMARY KEY NOT NULL,"
            " ID_CARD TEXT NOT NULL UNIQUE,"
            " NAME TEXT NOT NULL,"
            " SEX INTEGER NOT NULL DEFAULT 1,"
            " DOB TEXT,"
            " HEIGHT REAL,"
            " WEIGHT REAL,"
            " MOBILEPHONE TEXT,"
            " AGE INTEGER,"
            " CREATEDTIMESTAMP TEXT NOT NULL)"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS \"User\" ("
            " ID TEXT PRIMARY KEY NOT NULL,"
            " FULLNAME TEXT NOT NULL,"
            " USERNAME TEXT NOT NULL UNIQUE,"
            " PASSWORD TEXT NOT NULL)"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS History ("
            " ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            " USER_ID TEXT NOT NULL,"
            " EVENT TEXT NOT NULL,"
            " TIMESTAMP TEXT NOT NULL)"),
        QStringLiteral("CREATE INDEX IF NOT EXISTS idx_patient_name ON Patient(NAME)"),
        QStringLiteral("CREATE INDEX IF NOT EXISTS idx_patient_mobile ON Patient(MOBILEPHONE)")
    };

    for (const QString &sql : statements) {
        if (!runStatement(m_database, sql, error))
            return false;
    }
    return true;
}

bool DatabaseManager::seedDefaultData(QString *error)
{
    if (!m_database.transaction()) {
        if (error)
            *error = m_database.lastError().text();
        return false;
    }

    QSqlQuery query(m_database);
    query.exec(QStringLiteral("SELECT COUNT(*) FROM \"User\""));
    const bool userEmpty = query.next() && query.value(0).toInt() == 0;

    if (userEmpty) {
        query.prepare(QStringLiteral(
            "INSERT INTO \"User\" (ID, FULLNAME, USERNAME, PASSWORD) VALUES (?, ?, ?, ?)"));
        query.addBindValue(QStringLiteral("U001"));
        query.addBindValue(QStringLiteral("系统管理员"));
        query.addBindValue(QStringLiteral("admin"));
        query.addBindValue(passwordHash(QStringLiteral("123456")));
        if (!query.exec()) {
            if (error)
                *error = query.lastError().text();
            m_database.rollback();
            return false;
        }
    }

    query.exec(QStringLiteral("SELECT COUNT(*) FROM Department"));
    if (query.next() && query.value(0).toInt() == 0) {
        const QList<QPair<QString, QString>> departments = {
            {QStringLiteral("D01"), QStringLiteral("内科")},
            {QStringLiteral("D02"), QStringLiteral("外科")},
            {QStringLiteral("D03"), QStringLiteral("儿科")},
            {QStringLiteral("D04"), QStringLiteral("急诊科")}
        };
        for (const auto &department : departments) {
            query.prepare(QStringLiteral("INSERT INTO Department (ID, NAME) VALUES (?, ?)"));
            query.addBindValue(department.first);
            query.addBindValue(department.second);
            if (!query.exec()) {
                if (error)
                    *error = query.lastError().text();
                m_database.rollback();
                return false;
            }
        }
    }

    query.exec(QStringLiteral("SELECT COUNT(*) FROM Doctor"));
    if (query.next() && query.value(0).toInt() == 0) {
        const QList<QStringList> doctors = {
            {QStringLiteral("DR01"), QStringLiteral("E2001"), QStringLiteral("周明远"), QStringLiteral("D01")},
            {QStringLiteral("DR02"), QStringLiteral("E2002"), QStringLiteral("吴晓彤"), QStringLiteral("D02")},
            {QStringLiteral("DR03"), QStringLiteral("E2003"), QStringLiteral("何佳"), QStringLiteral("D03")},
            {QStringLiteral("DR04"), QStringLiteral("E2004"), QStringLiteral("赵立新"), QStringLiteral("D04")}
        };
        for (const QStringList &doctor : doctors) {
            query.prepare(QStringLiteral(
                "INSERT INTO Doctor (ID, EMPLOYEENO, NAME, DEPARTMENT_ID) VALUES (?, ?, ?, ?)"));
            query.addBindValue(doctor.at(0));
            query.addBindValue(doctor.at(1));
            query.addBindValue(doctor.at(2));
            query.addBindValue(doctor.at(3));
            if (!query.exec()) {
                if (error)
                    *error = query.lastError().text();
                m_database.rollback();
                return false;
            }
        }
    }

    query.exec(QStringLiteral("SELECT COUNT(*) FROM Patient"));
    if (query.next() && query.value(0).toInt() == 0) {
        const QList<QStringList> patients = {
            {QStringLiteral("P2026001"), QStringLiteral("441900199203121234"), QStringLiteral("陈志强"),
             QStringLiteral("1"), QStringLiteral("1992-03-12"), QStringLiteral("175.0"),
             QStringLiteral("70.5"), QStringLiteral("13800138001"), QStringLiteral("34")},
            {QStringLiteral("P2026002"), QStringLiteral("441900198805230028"), QStringLiteral("李慧敏"),
             QStringLiteral("0"), QStringLiteral("1988-05-23"), QStringLiteral("162.0"),
             QStringLiteral("54.0"), QStringLiteral("13900139002"), QStringLiteral("38")},
            {QStringLiteral("P2026003"), QStringLiteral("441900200107081516"), QStringLiteral("黄子轩"),
             QStringLiteral("1"), QStringLiteral("2001-07-08"), QStringLiteral("178.0"),
             QStringLiteral("66.0"), QStringLiteral("13700137003"), QStringLiteral("25")},
            {QStringLiteral("P2026004"), QStringLiteral("441900199512300043"), QStringLiteral("梁静怡"),
             QStringLiteral("0"), QStringLiteral("1995-12-30"), QStringLiteral("160.5"),
             QStringLiteral("50.0"), QStringLiteral("13600136004"), QStringLiteral("30")},
            {QStringLiteral("P2026005"), QStringLiteral("441900201802141027"), QStringLiteral("邓梓豪"),
             QStringLiteral("1"), QStringLiteral("2018-02-14"), QStringLiteral("122.0"),
             QStringLiteral("26.5"), QStringLiteral("13500135005"), QStringLiteral("8")},
            {QStringLiteral("P2026006"), QStringLiteral("441900197909251209"), QStringLiteral("罗秀兰"),
             QStringLiteral("0"), QStringLiteral("1979-09-25"), QStringLiteral("158.0"),
             QStringLiteral("57.5"), QStringLiteral("13400134006"), QStringLiteral("47")},
            {QStringLiteral("P2026007"), QStringLiteral("441900200310020017"), QStringLiteral("苏浩"),
             QStringLiteral("1"), QStringLiteral("2003-10-02"), QStringLiteral("180.0"),
             QStringLiteral("72.0"), QStringLiteral("13300133007"), QStringLiteral("23")},
            {QStringLiteral("P2026008"), QStringLiteral("441900199901180048"), QStringLiteral("叶诗敏"),
             QStringLiteral("0"), QStringLiteral("1999-01-18"), QStringLiteral("165.0"),
             QStringLiteral("52.0"), QStringLiteral("13200132008"), QStringLiteral("27")}
        };
        for (const QStringList &patient : patients) {
            query.prepare(QStringLiteral(
                "INSERT INTO Patient (ID, ID_CARD, NAME, SEX, DOB, HEIGHT, WEIGHT,"
                " MOBILEPHONE, AGE, CREATEDTIMESTAMP)"
                " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
            query.addBindValue(patient.at(0));
            query.addBindValue(patient.at(1));
            query.addBindValue(patient.at(2));
            query.addBindValue(patient.at(3).toInt());
            query.addBindValue(patient.at(4));
            query.addBindValue(patient.at(5).toDouble());
            query.addBindValue(patient.at(6).toDouble());
            query.addBindValue(patient.at(7));
            query.addBindValue(patient.at(8).toInt());
            query.addBindValue(QStringLiteral("2026-11-18 09:20:00"));
            if (!query.exec()) {
                if (error)
                    *error = query.lastError().text();
                m_database.rollback();
                return false;
            }
        }
    }

    if (!m_database.commit()) {
        if (error)
            *error = m_database.lastError().text();
        return false;
    }
    return true;
}

QString DatabaseManager::defaultDatabasePath()
{
    return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("hospital.db"));
}

QString DatabaseManager::passwordHash(const QString &plain)
{
    return QString::fromLatin1(
        QCryptographicHash::hash(plain.toUtf8(), QCryptographicHash::Sha256).toHex());
}

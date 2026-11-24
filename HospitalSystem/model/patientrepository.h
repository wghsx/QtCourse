#ifndef PATIENTREPOSITORY_H
#define PATIENTREPOSITORY_H

#include <QString>

struct PatientRecord
{
    QString id;
    QString idCard;
    QString name;
    int sex = 1;
    QString dob;
    double height = 0.0;
    double weight = 0.0;
    QString mobile;
    int age = 0;
    QString createdTimestamp;
};

// 患者表的写操作集中在仓库层，界面只负责收集数据与提示结果。
class PatientRepository
{
public:
    static bool insert(const PatientRecord &record, QString *error = nullptr);
    static bool update(const PatientRecord &record, QString *error = nullptr);
    static bool remove(const QString &id, QString *error = nullptr);
    static int count(QString *error = nullptr);
    static bool idCardExists(const QString &idCard, const QString &excludeId,
                             bool *exists, QString *error = nullptr);
};

#endif // PATIENTREPOSITORY_H

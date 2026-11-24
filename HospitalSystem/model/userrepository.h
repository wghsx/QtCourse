#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include <QString>

class UserRepository
{
public:
    static bool authenticate(const QString &username, const QString &password,
                             QString *userId, QString *fullName, QString *error = nullptr);
    static bool exists(const QString &username, bool *exists, QString *error = nullptr);
    static bool create(const QString &fullName, const QString &username,
                       const QString &password, QString *error = nullptr);
};

#endif // USERREPOSITORY_H

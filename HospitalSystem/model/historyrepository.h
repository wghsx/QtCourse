#ifndef HISTORYREPOSITORY_H
#define HISTORYREPOSITORY_H

#include <QString>

// 操作记录（选做项）：登录、增删改都写一条，便于事后核对数据变化。
class HistoryRepository
{
public:
    static bool record(const QString &userId, const QString &event);
};

#endif // HISTORYREPOSITORY_H

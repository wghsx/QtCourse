#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>

class QSqlQueryModel;
class QTableView;

// 操作记录（选做）：用 QSqlQueryModel 做一次联表查询，按时间倒序显示最近 200 条。
class HistoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HistoryDialog(QWidget *parent = nullptr);

private:
    QSqlQueryModel *m_model = nullptr;
    QTableView *m_table = nullptr;
};

#endif // HISTORYDIALOG_H

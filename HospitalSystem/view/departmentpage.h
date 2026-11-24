#ifndef DEPARTMENTPAGE_H
#define DEPARTMENTPAGE_H

#include <QWidget>

class QLabel;
class QSqlQueryModel;
class QTableView;

// 科室管理（选做）：只读列表，显示科室与各科医生人数。
class DepartmentPage : public QWidget
{
    Q_OBJECT
public:
    explicit DepartmentPage(QWidget *parent = nullptr);
    void refresh();

private:
    QSqlQueryModel *m_model = nullptr;
    QTableView *m_table = nullptr;
    QLabel *m_summaryLabel = nullptr;
};

#endif // DEPARTMENTPAGE_H

#ifndef DOCTORPAGE_H
#define DOCTORPAGE_H

#include <QWidget>

class QLabel;
class QSqlQueryModel;
class QTableView;

// 医生管理（选做）：只读列表，显示工号与所属科室。
class DoctorPage : public QWidget
{
    Q_OBJECT
public:
    explicit DoctorPage(QWidget *parent = nullptr);
    void refresh();

private:
    QSqlQueryModel *m_model = nullptr;
    QTableView *m_table = nullptr;
    QLabel *m_summaryLabel = nullptr;
};

#endif // DOCTORPAGE_H

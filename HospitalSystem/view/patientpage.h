#ifndef PATIENTPAGE_H
#define PATIENTPAGE_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QModelIndex;
class QPushButton;
class QSqlTableModel;
class QTableView;

// 患者管理页：QSqlTableModel 提供数据，QTableView 负责显示，
// 增删改操作统一走 PatientRepository。
class PatientPage : public QWidget
{
    Q_OBJECT
public:
    explicit PatientPage(QWidget *parent = nullptr);

    void setCurrentUser(const QString &userId, const QString &fullName);
    void refresh();

private slots:
    void applyFilter();
    void clearFilter();
    void addPatient();
    void editPatient();
    void deletePatient();
    void updateActions();
    void submitInlineEdit();

private:
    void setupModel();
    void updateSummary(const QString &keyword);
    QModelIndex currentRow() const;

    QSqlTableModel *m_model = nullptr;
    QTableView *m_table = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QLabel *m_summaryLabel = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_editButton = nullptr;
    QPushButton *m_deleteButton = nullptr;

    QString m_userId;
    QString m_userName;
    bool m_submitting = false;
};

#endif // PATIENTPAGE_H

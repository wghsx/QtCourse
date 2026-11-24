#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H

#include <QDialog>

#include "model/patientrepository.h"

class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QSpinBox;

// 患者新增 / 修改表单：所有校验在提交前完成，保存失败时对话框不关闭。
class PatientDialog : public QDialog
{
    Q_OBJECT
public:
    enum Mode { AddMode, EditMode };

    explicit PatientDialog(Mode mode, QWidget *parent = nullptr);

    void setRecord(const PatientRecord &record);
    PatientRecord record() const;

protected:
    void accept() override;

private:
    bool validate(QString *message) const;
    void updateAgeFromDob();

    Mode m_mode;
    QString m_createdTimestamp;

    QLineEdit *m_idEdit = nullptr;
    QLineEdit *m_idCardEdit = nullptr;
    QLineEdit *m_nameEdit = nullptr;
    QComboBox *m_sexBox = nullptr;
    QDateEdit *m_dobEdit = nullptr;
    QDoubleSpinBox *m_heightBox = nullptr;
    QDoubleSpinBox *m_weightBox = nullptr;
    QLineEdit *m_mobileEdit = nullptr;
    QSpinBox *m_ageBox = nullptr;
    QLabel *m_createdLabel = nullptr;
    QLabel *m_errorLabel = nullptr;
};

#endif // PATIENTDIALOG_H

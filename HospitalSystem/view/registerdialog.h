#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;

// 注册对话框：新增一个可登录的账号，用户名唯一。
class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr);

private slots:
    void submit();

private:
    QLineEdit *m_fullNameEdit = nullptr;
    QLineEdit *m_usernameEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    QLineEdit *m_confirmEdit = nullptr;
    QLabel *m_errorLabel = nullptr;
};

#endif // REGISTERDIALOG_H

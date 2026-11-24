#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

namespace Ui {
class LoginPage;
}

// 登录页：只负责收集凭据、调用仓库层校验、把结果通过信号交给主窗口。
class LoginPage : public QWidget
{
    Q_OBJECT
public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage() override;

    void reset();

signals:
    void loginSucceeded(const QString &userId, const QString &fullName, const QString &username);

private slots:
    void tryLogin();
    void openRegister();

private:
    Ui::LoginPage *ui;
};

#endif // LOGINPAGE_H

#include "loginpage.h"
#include "ui_loginpage.h"

#include "model/historyrepository.h"
#include "model/userrepository.h"
#include "registerdialog.h"

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    ui->btnLogin->setObjectName(QStringLiteral("primaryButton"));

    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginPage::tryLogin);
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginPage::openRegister);
    connect(ui->edtUsername, &QLineEdit::returnPressed, this, &LoginPage::tryLogin);
    connect(ui->edtPassword, &QLineEdit::returnPressed, this, &LoginPage::tryLogin);

    ui->edtUsername->setFocus();
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::reset()
{
    ui->edtPassword->clear();
    ui->lblError->clear();
    ui->edtUsername->setFocus();
    ui->edtUsername->selectAll();
}

void LoginPage::tryLogin()
{
    const QString username = ui->edtUsername->text().trimmed();
    const QString password = ui->edtPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->lblError->setText(QStringLiteral("用户名和密码不能为空。"));
        return;
    }

    QString userId;
    QString fullName;
    QString error;
    if (!UserRepository::authenticate(username, password, &userId, &fullName, &error)) {
        if (error.isEmpty()) {
            ui->lblError->setText(QStringLiteral("用户名或密码错误，请重新输入。"));
            HistoryRepository::record(QStringLiteral("U000"),
                                      QStringLiteral("登录失败，用户名：%1").arg(username));
        } else {
            ui->lblError->setText(QStringLiteral("登录失败：%1").arg(error));
        }
        ui->edtPassword->selectAll();
        ui->edtPassword->setFocus();
        return;
    }

    ui->lblError->clear();
    ui->edtPassword->clear();
    HistoryRepository::record(userId, QStringLiteral("登录成功"));
    emit loginSucceeded(userId, fullName, username);
}

void LoginPage::openRegister()
{
    RegisterDialog dialog(this);
    dialog.exec();
}

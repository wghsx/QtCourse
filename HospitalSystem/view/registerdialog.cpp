#include "registerdialog.h"

#include "model/userrepository.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("注册账号"));
    setModal(true);
    setMinimumWidth(420);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 24, 28, 20);
    layout->setSpacing(12);

    auto *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setSpacing(12);

    m_fullNameEdit = new QLineEdit(this);
    m_fullNameEdit->setPlaceholderText(QStringLiteral("例如：张三"));
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText(QStringLiteral("字母或数字，4 位以上"));
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText(QStringLiteral("至少 6 位"));
    m_confirmEdit = new QLineEdit(this);
    m_confirmEdit->setEchoMode(QLineEdit::Password);

    form->addRow(QStringLiteral("姓名"), m_fullNameEdit);
    form->addRow(QStringLiteral("用户名"), m_usernameEdit);
    form->addRow(QStringLiteral("密码"), m_passwordEdit);
    form->addRow(QStringLiteral("确认密码"), m_confirmEdit);
    layout->addLayout(form);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName(QStringLiteral("formError"));
    m_errorLabel->setWordWrap(true);
    layout->addWidget(m_errorLabel);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    auto *submitButton = buttons->addButton(QStringLiteral("注册"), QDialogButtonBox::AcceptRole);
    submitButton->setObjectName(QStringLiteral("primaryButton"));
    submitButton->setDefault(true);
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::rejected, this, &RegisterDialog::reject);
    connect(submitButton, &QPushButton::clicked, this, &RegisterDialog::submit);
    connect(m_confirmEdit, &QLineEdit::returnPressed, this, &RegisterDialog::submit);
}

void RegisterDialog::submit()
{
    const QString fullName = m_fullNameEdit->text().trimmed();
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text();
    const QString confirm = m_confirmEdit->text();

    if (fullName.isEmpty()) {
        m_errorLabel->setText(QStringLiteral("请填写姓名。"));
        m_fullNameEdit->setFocus();
        return;
    }
    if (!QRegularExpression(QStringLiteral("^[A-Za-z0-9_]{4,20}$")).match(username).hasMatch()) {
        m_errorLabel->setText(QStringLiteral("用户名需为 4～20 位字母、数字或下划线。"));
        m_usernameEdit->setFocus();
        return;
    }
    if (password.size() < 6) {
        m_errorLabel->setText(QStringLiteral("密码长度不能少于 6 位。"));
        m_passwordEdit->setFocus();
        return;
    }
    if (password != confirm) {
        m_errorLabel->setText(QStringLiteral("两次输入的密码不一致。"));
        m_confirmEdit->setFocus();
        return;
    }

    bool exists = false;
    QString error;
    if (!UserRepository::exists(username, &exists, &error)) {
        m_errorLabel->setText(QStringLiteral("检查用户名失败：%1").arg(error));
        return;
    }
    if (exists) {
        m_errorLabel->setText(QStringLiteral("该用户名已被占用，请更换。"));
        m_usernameEdit->setFocus();
        return;
    }
    if (!UserRepository::create(fullName, username, password, &error)) {
        m_errorLabel->setText(error);
        return;
    }

    QMessageBox::information(this, QStringLiteral("注册成功"),
                             QStringLiteral("账号 %1 已创建，请返回登录。").arg(username));
    accept();
}

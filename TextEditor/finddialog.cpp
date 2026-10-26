#include "finddialog.h"
#include "ui_finddialog.h"

#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextDocument>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("查找"));
    ui->lblMsg->clear();
    connect(ui->btnFindNext, &QPushButton::clicked, this, &FindDialog::onFindNext);
    connect(ui->btnCancel, &QPushButton::clicked, this, &FindDialog::close);
    connect(ui->editFind, &QLineEdit::returnPressed, this, &FindDialog::onFindNext);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    focusFind();
}

void FindDialog::focusFind()
{
    ui->editFind->setFocus();
    ui->editFind->selectAll();
}

void FindDialog::onFindNext()
{
    if (!m_editor)
        return;

    const QString text = ui->editFind->text();
    if (text.isEmpty()) {
        ui->lblMsg->setText(QStringLiteral("请输入查找内容"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (ui->chkCase->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    const bool backward = ui->radioUp->isChecked();
    if (backward)
        flags |= QTextDocument::FindBackward;

    QTextDocument *doc = m_editor->document();
    QTextCursor cursor = m_editor->textCursor();
    QTextCursor found = doc->find(text, cursor, flags);

    bool wrapped = false;
    if (found.isNull()) {
        cursor.movePosition(backward ? QTextCursor::End : QTextCursor::Start);
        found = doc->find(text, cursor, flags);
        wrapped = !found.isNull();
    }

    if (!found.isNull()) {
        m_editor->setTextCursor(found);
        m_editor->ensureCursorVisible();
        ui->lblMsg->setText(wrapped ? QStringLiteral("已从头继续查找") : QString());
    } else {
        ui->lblMsg->setText(QStringLiteral("未找到 \"%1\"").arg(text));
    }
}

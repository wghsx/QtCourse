#include "replacedialog.h"
#include "ui_replacedialog.h"

#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextDocument>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("替换"));
    ui->lblMsg->clear();
    connect(ui->btnFindNext, &QPushButton::clicked, this, &ReplaceDialog::onFindNext);
    connect(ui->btnReplace, &QPushButton::clicked, this, &ReplaceDialog::onReplace);
    connect(ui->btnReplaceAll, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAll);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ReplaceDialog::close);
    connect(ui->editFind, &QLineEdit::returnPressed, this, &ReplaceDialog::onFindNext);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    focusFind();
}

void ReplaceDialog::focusFind()
{
    ui->editFind->setFocus();
    ui->editFind->selectAll();
}

bool ReplaceDialog::matchSelected(const QString &selected) const
{
    const QString target = ui->editFind->text();
    if (ui->chkCase->isChecked())
        return selected == target;
    return selected.compare(target, Qt::CaseInsensitive) == 0;
}

bool ReplaceDialog::findNextInternal()
{
    if (!m_editor)
        return false;

    const QString text = ui->editFind->text();
    if (text.isEmpty()) {
        ui->lblMsg->setText(QStringLiteral("请输入查找内容"));
        return false;
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
        return true;
    }

    ui->lblMsg->setText(QStringLiteral("未找到 \"%1\"").arg(text));
    return false;
}

void ReplaceDialog::onFindNext()
{
    findNextInternal();
}

void ReplaceDialog::onReplace()
{
    if (!m_editor)
        return;

    QTextCursor cursor = m_editor->textCursor();
    if (cursor.hasSelection() && matchSelected(cursor.selectedText())) {
        cursor.insertText(ui->editReplace->text());
        m_editor->setTextCursor(cursor);
        ui->lblMsg->setText(QStringLiteral("已替换 1 处"));
    }
    findNextInternal();
}

void ReplaceDialog::onReplaceAll()
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

    QTextDocument *doc = m_editor->document();
    QTextCursor editCursor = m_editor->textCursor();
    editCursor.beginEditBlock();

    int count = 0;
    QTextCursor found = doc->find(text, 0, flags);
    while (!found.isNull()) {
        found.insertText(ui->editReplace->text());
        ++count;
        found = doc->find(text, found, flags);
    }

    editCursor.endEditBlock();

    if (count > 0)
        ui->lblMsg->setText(QStringLiteral("已替换 %1 处").arg(count));
    else
        ui->lblMsg->setText(QStringLiteral("未找到 \"%1\"").arg(text));
}

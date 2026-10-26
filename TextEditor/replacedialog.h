#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ReplaceDialog; }
QT_END_NAMESPACE

class QPlainTextEdit;

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

    void setEditor(QPlainTextEdit *editor) { m_editor = editor; }
    void focusFind();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();

private:
    bool findNextInternal();
    bool matchSelected(const QString &selected) const;

    Ui::ReplaceDialog *ui;
    QPlainTextEdit *m_editor = nullptr;
};

#endif // REPLACEDIALOG_H

#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class FindDialog; }
QT_END_NAMESPACE

class QPlainTextEdit;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    void setEditor(QPlainTextEdit *editor) { m_editor = editor; }
    void focusFind();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onFindNext();

private:
    Ui::FindDialog *ui;
    QPlainTextEdit *m_editor = nullptr;
};

#endif // FINDDIALOG_H

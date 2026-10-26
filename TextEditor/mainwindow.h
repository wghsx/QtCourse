#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CodeEditor;
class FindDialog;
class ReplaceDialog;
class QLabel;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveAsFile();
    void about();

    void updateTitle();
    void updateStatus();
    void updateActions();

    void showFindDialog();
    void showReplaceDialog();
    void chooseFont();
    void chooseFontColor();
    void chooseFontBgColor();
    void chooseEditorBgColor();

    void toggleWrap(bool checked);
    void toggleLineNumbers(bool checked);
    void toggleToolBar(bool checked);
    void toggleStatusBar(bool checked);
    void toggleAutoSave(bool checked);
    void autoSaveTick();

private:
    void setupEditor();
    void setupActions();
    void setupStatusBar();
    void mergeCharFormat(const QTextCharFormat &format);
    bool maybeSave();
    void loadFile(const QString &path);
    void setCurrentFile(const QString &path);

    Ui::MainWindow *ui;
    QString m_currentFile;
    bool m_autoSaveEnabled = false;
    QTimer *m_autoSaveTimer = nullptr;

    QLabel *m_lengthLabel = nullptr;
    QLabel *m_linesLabel = nullptr;
    QLabel *m_posLabel = nullptr;
    QLabel *m_nameLabel = nullptr;

    FindDialog *m_findDialog = nullptr;
    ReplaceDialog *m_replaceDialog = nullptr;
};

#endif // MAINWINDOW_H

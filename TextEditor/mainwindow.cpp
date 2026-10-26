#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "codeeditor.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "aboutdialog.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QLabel>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTimer>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupEditor();
    setupActions();
    setupStatusBar();

    m_findDialog = new FindDialog(this);
    m_findDialog->setEditor(ui->textEdit);
    m_replaceDialog = new ReplaceDialog(this);
    m_replaceDialog->setEditor(ui->textEdit);

    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(30000); // 30 秒
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSaveTick);

    updateTitle();
    updateStatus();
    updateActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupEditor()
{
    ui->textEdit->setFont(QFont(QStringLiteral("Consolas"), 12));
    ui->textEdit->setTabStopDistance(4 * ui->textEdit->fontMetrics().horizontalAdvance(QLatin1Char(' ')));
    ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::updateTitle);
    connect(ui->textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::updateStatus);
    connect(ui->textEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateStatus);
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable, this, &MainWindow::updateActions);
    connect(ui->textEdit->document(), &QTextDocument::undoAvailable, this, &MainWindow::updateActions);
    connect(ui->textEdit->document(), &QTextDocument::redoAvailable, this, &MainWindow::updateActions);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::updateActions);
}

void MainWindow::setupActions()
{
    ui->actionNew->setIcon(QIcon(QStringLiteral(":/icons/new.png")));
    ui->actionOpen->setIcon(QIcon(QStringLiteral(":/icons/open.png")));
    ui->actionSave->setIcon(QIcon(QStringLiteral(":/icons/save.png")));
    ui->actionSaveAs->setIcon(QIcon(QStringLiteral(":/icons/saveas.png")));
    ui->actionUndo->setIcon(QIcon(QStringLiteral(":/icons/undo.png")));
    ui->actionRedo->setIcon(QIcon(QStringLiteral(":/icons/redo.png")));
    ui->actionCut->setIcon(QIcon(QStringLiteral(":/icons/cut.png")));
    ui->actionCopy->setIcon(QIcon(QStringLiteral(":/icons/copy.png")));
    ui->actionPaste->setIcon(QIcon(QStringLiteral(":/icons/paste.png")));
    ui->actionFind->setIcon(QIcon(QStringLiteral(":/icons/find.png")));
    ui->actionReplace->setIcon(QIcon(QStringLiteral(":/icons/replace.png")));
    ui->actionWrap->setIcon(QIcon(QStringLiteral(":/icons/wrap.png")));
    ui->actionShowLine->setIcon(QIcon(QStringLiteral(":/icons/line.png")));
    ui->actionFont->setIcon(QIcon(QStringLiteral(":/icons/font.png")));
    ui->actionFontColor->setIcon(QIcon(QStringLiteral(":/icons/fontcolor.png")));
    ui->actionFontBgColor->setIcon(QIcon(QStringLiteral(":/icons/fontbg.png")));
    ui->actionEditorBgColor->setIcon(QIcon(QStringLiteral(":/icons/editbg.png")));
    ui->actionAbout->setIcon(QIcon(QStringLiteral(":/icons/about.png")));

    ui->toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->toolBar->setIconSize(QSize(22, 22));

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->textEdit, &QPlainTextEdit::selectAll);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::showFindDialog);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::showReplaceDialog);

    connect(ui->actionWrap, &QAction::toggled, this, &MainWindow::toggleWrap);
    connect(ui->actionShowLine, &QAction::toggled, this, &MainWindow::toggleLineNumbers);
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::chooseFont);
    connect(ui->actionFontColor, &QAction::triggered, this, &MainWindow::chooseFontColor);
    connect(ui->actionFontBgColor, &QAction::triggered, this, &MainWindow::chooseFontBgColor);
    connect(ui->actionEditorBgColor, &QAction::triggered, this, &MainWindow::chooseEditorBgColor);
    connect(ui->actionAutoSave, &QAction::toggled, this, &MainWindow::toggleAutoSave);

    connect(ui->actionToolBar, &QAction::toggled, this, &MainWindow::toggleToolBar);
    connect(ui->actionStatusBar, &QAction::toggled, this, &MainWindow::toggleStatusBar);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);

    // 保证中文菜单项完整显示，避免被省略号截断
    ui->menubar->setStyleSheet(QStringLiteral("QMenu { min-width: 175px; }"));
}

void MainWindow::setupStatusBar()
{
    m_lengthLabel = new QLabel(this);
    m_linesLabel = new QLabel(this);
    m_posLabel = new QLabel(this);
    m_nameLabel = new QLabel(QStringLiteral("黄圣杰"), this);

    statusBar()->addWidget(m_lengthLabel);
    statusBar()->addWidget(m_linesLabel);
    statusBar()->addPermanentWidget(m_posLabel);
    statusBar()->addPermanentWidget(m_nameLabel);
}

void MainWindow::updateTitle()
{
    const QString name = m_currentFile.isEmpty()
            ? QStringLiteral("新建文本文件")
            : QFileInfo(m_currentFile).fileName();
    const QString star = ui->textEdit->document()->isModified() ? QStringLiteral("*") : QString();
    setWindowTitle(star + name + QStringLiteral(" - 编辑器"));
}

void MainWindow::updateStatus()
{
    const int length = ui->textEdit->toPlainText().length();
    const int lines = ui->textEdit->document()->blockCount();
    const QTextCursor cursor = ui->textEdit->textCursor();

    m_lengthLabel->setText(QStringLiteral("length: %1").arg(length));
    m_linesLabel->setText(QStringLiteral("lines: %1").arg(lines));
    m_posLabel->setText(QStringLiteral("Ln: %1  Col: %2")
                        .arg(cursor.blockNumber() + 1)
                        .arg(cursor.positionInBlock() + 1));
}

void MainWindow::updateActions()
{
    const bool hasSelection = ui->textEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionPaste->setEnabled(ui->textEdit->canPaste());
    ui->actionUndo->setEnabled(ui->textEdit->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->textEdit->document()->isRedoAvailable());
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret = QMessageBox::warning(
                this, QStringLiteral("未保存的更改"),
                QStringLiteral("文件已被修改，是否保存更改？"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return saveFile();
    if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void MainWindow::newFile()
{
    if (!maybeSave())
        return;
    ui->textEdit->clear();
    setCurrentFile(QString());
}

void MainWindow::openFile()
{
    if (!maybeSave())
        return;

    const QString path = QFileDialog::getOpenFileName(
                this, QStringLiteral("打开文件"), QString(),
                QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"),
                nullptr, QFileDialog::DontUseNativeDialog);
    if (path.isEmpty())
        return;
    loadFile(path);
}

void MainWindow::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.exists()) {
        QMessageBox::warning(this, QStringLiteral("打开失败"),
                             QStringLiteral("文件不存在：\n%1").arg(path));
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("打开失败"),
                             QStringLiteral("无法打开文件：\n%1\n%2").arg(path, file.errorString()));
        return;
    }

    const QByteArray data = file.readAll();
    file.close();

    ui->textEdit->setPlainText(QString::fromUtf8(data));
    setCurrentFile(path);
    statusBar()->showMessage(QStringLiteral("已打开：%1").arg(QFileInfo(path).fileName()), 3000);
}

bool MainWindow::saveFile()
{
    if (m_currentFile.isEmpty())
        return saveAsFile();

    QFile file(m_currentFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"),
                             QStringLiteral("无法写入文件：\n%1\n%2")
                             .arg(m_currentFile, file.errorString()));
        return false;
    }

    file.write(ui->textEdit->toPlainText().toUtf8());
    file.close();

    ui->textEdit->document()->setModified(false);
    updateTitle();
    statusBar()->showMessage(QStringLiteral("已保存：%1").arg(QFileInfo(m_currentFile).fileName()), 3000);
    return true;
}

bool MainWindow::saveAsFile()
{
    QString path = QFileDialog::getSaveFileName(
                this, QStringLiteral("另存为"), m_currentFile,
                QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"),
                nullptr, QFileDialog::DontUseNativeDialog);
    if (path.isEmpty())
        return false;

    m_currentFile = path;
    return saveFile();
}

void MainWindow::setCurrentFile(const QString &path)
{
    m_currentFile = path;
    ui->textEdit->document()->setModified(false);
    updateTitle();
    updateActions();
}

void MainWindow::showFindDialog()
{
    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
    m_findDialog->focusFind();
}

void MainWindow::showReplaceDialog()
{
    m_replaceDialog->show();
    m_replaceDialog->raise();
    m_replaceDialog->activateWindow();
    m_replaceDialog->focusFind();
}

void MainWindow::chooseFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, ui->textEdit->font(), this,
                                            QStringLiteral("选择字体"));
    if (ok)
        ui->textEdit->setFont(font);
}

void MainWindow::mergeCharFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (cursor.hasSelection())
        cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::chooseFontColor()
{
    const QColor current = ui->textEdit->currentCharFormat().foreground().color();
    const QColor color = QColorDialog::getColor(current, this, QStringLiteral("字体颜色"));
    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setForeground(color);
    mergeCharFormat(format);
}

void MainWindow::chooseFontBgColor()
{
    const QColor color = QColorDialog::getColor(Qt::yellow, this, QStringLiteral("字体背景色"));
    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setBackground(color);
    mergeCharFormat(format);
}

void MainWindow::chooseEditorBgColor()
{
    const QColor color = QColorDialog::getColor(Qt::white, this,
                                                QStringLiteral("编辑器背景色"));
    if (color.isValid()) {
        ui->textEdit->setStyleSheet(QStringLiteral("QPlainTextEdit { background-color: %1; }")
                                    .arg(color.name()));
    }
}

void MainWindow::toggleWrap(bool checked)
{
    ui->textEdit->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth
                                          : QPlainTextEdit::NoWrap);
}

void MainWindow::toggleLineNumbers(bool checked)
{
    ui->textEdit->setLineNumbersVisible(checked);
}

void MainWindow::toggleToolBar(bool checked)
{
    ui->toolBar->setVisible(checked);
}

void MainWindow::toggleStatusBar(bool checked)
{
    statusBar()->setVisible(checked);
}

void MainWindow::toggleAutoSave(bool checked)
{
    m_autoSaveEnabled = checked;
    if (checked) {
        m_autoSaveTimer->start();
        statusBar()->showMessage(QStringLiteral("已开启定时自动保存（每 30 秒）"), 3000);
    } else {
        m_autoSaveTimer->stop();
        statusBar()->showMessage(QStringLiteral("已关闭定时自动保存"), 3000);
    }
}

void MainWindow::autoSaveTick()
{
    if (!m_autoSaveEnabled)
        return;
    if (m_currentFile.isEmpty() || !ui->textEdit->document()->isModified())
        return;
    saveFile();
}

void MainWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

#include <QApplication>
#include <QDir>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QtTest>
#include "calculatorwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CalculatorWindow window;
    window.resize(700, 720);
    window.move(340, 120);
    window.show();
    window.raise();
    window.activateWindow();
    QTest::qWait(6000);

    const auto click = [&](const char *name) {
        auto *button = window.findChild<QPushButton *>(name);
        if (button) QTest::mouseClick(button, Qt::LeftButton);
        QTest::qWait(1800);
    };
    const auto key = [&](Qt::Key code) {
        QTest::keyClick(&window, code);
        QTest::qWait(1800);
    };

    // Mouse: decimal arithmetic.
    click("oneButton"); click("twoButton"); click("decimalButton");
    click("fiveButton"); click("addButton"); click("threeButton");
    click("decimalButton"); click("fiveButton"); click("equalsButton");
    QTest::qWait(3000);

    // Keyboard: arithmetic, backspace and clear.
    key(Qt::Key_Escape); key(Qt::Key_8); key(Qt::Key_4);
    key(Qt::Key_Slash); key(Qt::Key_7); key(Qt::Key_Return);
    QTest::qWait(3000);
    key(Qt::Key_1); key(Qt::Key_2); key(Qt::Key_3);
    key(Qt::Key_Backspace); key(Qt::Key_Escape);
    QTest::qWait(3000);

    // Divide by zero and continue from the error.
    key(Qt::Key_8); key(Qt::Key_Slash); key(Qt::Key_0); key(Qt::Key_Return);
    QTest::qWait(3500);
    key(Qt::Key_2); key(Qt::Key_Plus); key(Qt::Key_3); key(Qt::Key_Return);
    QTest::qWait(2500);

    // Continuous operations and abnormal repeated input.
    key(Qt::Key_Escape); key(Qt::Key_2); key(Qt::Key_Plus);
    key(Qt::Key_3); key(Qt::Key_Asterisk); key(Qt::Key_4); key(Qt::Key_Return);
    QTest::qWait(3000);
    key(Qt::Key_Escape); key(Qt::Key_1); key(Qt::Key_Period);
    key(Qt::Key_Period); key(Qt::Key_2); key(Qt::Key_Plus);
    key(Qt::Key_Minus); key(Qt::Key_3); key(Qt::Key_Return);
    QTest::qWait(3000);

    // Show the real repository history in the same continuous recording.
    QDir repo(QCoreApplication::applicationDirPath());
    repo.cdUp(); repo.cdUp(); repo.cdUp();
    QProcess git;
    git.start("git", {"-C", repo.absolutePath(), "log", "--oneline", "-6"});
    git.waitForFinished(5000);
    QPlainTextEdit history(&window);
    history.setGeometry(20, 120, window.width() - 40, window.height() - 145);
    history.setReadOnly(true);
    history.setStyleSheet("background: white; color: #101828; font-size: 18px;");
    history.setPlainText("QtCourse / Calculator Git 提交历史\n\n" +
                         QString::fromUtf8(git.readAllStandardOutput()));
    history.show();
    history.raise();
    QTest::qWait(25000);
    return 0;
}

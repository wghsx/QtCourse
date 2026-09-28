#include <QtTest>
#include <QLabel>
#include <QPushButton>
#include "calculatorengine.h"
#include "calculatorwindow.h"

class CalculatorTests : public QObject
{
    Q_OBJECT
private slots:
    void arithmetic();
    void repeatedDecimalAndOperator();
    void divideByZeroAndRecovery();
    void backspaceClearAndResult();
    void mouseKeyboardSharedLogic();
};

static void enter(CalculatorEngine &engine, const QStringList &keys)
{
    for (const QString &key : keys) engine.input(key);
}

void CalculatorTests::arithmetic()
{
    CalculatorEngine e;
    enter(e, {"1", "2", ".", "5", "+", "3", ".", "5", "="});
    QCOMPARE(e.display(), QString("16"));
    enter(e, {"×", "2", "-", "4", "÷", "7", "="});
    QCOMPARE(e.display(), QString("4"));
}

void CalculatorTests::repeatedDecimalAndOperator()
{
    CalculatorEngine e;
    enter(e, {"1", ".", ".", "2", "+", "-", "3", "="});
    QCOMPARE(e.display(), QString("-1.8"));
    QCOMPARE(e.expression(), QString("1.2 - 3 ="));
}

void CalculatorTests::divideByZeroAndRecovery()
{
    CalculatorEngine e;
    enter(e, {"8", "÷", "0", "="});
    QCOMPARE(e.display(), QString("不能除以零"));
    enter(e, {"2", "+", "3", "="});
    QCOMPARE(e.display(), QString("5"));
}

void CalculatorTests::backspaceClearAndResult()
{
    CalculatorEngine e;
    enter(e, {"1", "2", "⌫", "+", "2", "="});
    QCOMPARE(e.display(), QString("3"));
    enter(e, {"4", "+", "5", "="});
    QCOMPARE(e.display(), QString("9"));
    enter(e, {"C", "7", "-", "2", "="});
    QCOMPARE(e.display(), QString("5"));
}

void CalculatorTests::mouseKeyboardSharedLogic()
{
    CalculatorWindow window;
    window.show();
    auto *one = window.findChild<QPushButton *>("oneButton");
    auto *add = window.findChild<QPushButton *>("addButton");
    auto *display = window.findChild<QLabel *>("displayLabel");
    QVERIFY(one && add && display);
    QTest::mouseClick(one, Qt::LeftButton);
    QTest::mouseClick(add, Qt::LeftButton);
    QTest::keyClick(&window, Qt::Key_2);
    QTest::keyClick(&window, Qt::Key_Return);
    QCOMPARE(display->text(), QString("3"));
    QTest::keyClick(&window, Qt::Key_Escape);
    QCOMPARE(display->text(), QString("0"));
    QTest::keyClicks(&window, "12.5+3.5");
    QTest::keyClick(&window, Qt::Key_Return);
    QCOMPARE(display->text(), QString("16"));
    QVERIFY(window.grab().save("../calculator-screenshot.png"));
}

QTEST_MAIN(CalculatorTests)
#include "tests.moc"

#include "calculatorwindow.h"
#include "ui_calculatorwindow.h"

#include <QKeyEvent>
#include <QPushButton>

CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::CalculatorWindow)
{
    ui->setupUi(this);
    const auto buttons = findChildren<QPushButton *>();
    for (auto *button : buttons) {
        button->setFocusPolicy(Qt::NoFocus);
        connect(button, &QPushButton::clicked, this, [this, button] {
            dispatch(button->text());
        });
    }
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    dispatch(QString());
}

CalculatorWindow::~CalculatorWindow() { delete ui; }

void CalculatorWindow::dispatch(const QString &key)
{
    m_engine.input(key);
    ui->displayLabel->setText(m_engine.display());
    ui->expressionLabel->setText(m_engine.expression());
}

void CalculatorWindow::keyPressEvent(QKeyEvent *event)
{
    QString key;
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
        key = QString::number(event->key() - Qt::Key_0);
    else if (event->key() == Qt::Key_Period || event->key() == Qt::Key_Comma) key = ".";
    else if (event->key() == Qt::Key_Plus) key = "+";
    else if (event->key() == Qt::Key_Minus) key = "-";
    else if (event->key() == Qt::Key_Asterisk || event->key() == Qt::Key_X) key = "×";
    else if (event->key() == Qt::Key_Slash) key = "÷";
    else if (event->key() == Qt::Key_Equal || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) key = "=";
    else if (event->key() == Qt::Key_Backspace) key = "⌫";
    else if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Delete) key = "C";
    if (key.isEmpty()) QWidget::keyPressEvent(event);
    else { dispatch(key); event->accept(); }
}

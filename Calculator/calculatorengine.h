#pragma once

#include <QString>

class CalculatorEngine
{
public:
    void input(const QString &key);
    QString display() const { return m_display; }
    QString expression() const { return m_expression; }

private:
    void digit(const QString &value);
    void decimalPoint();
    void operation(const QString &value);
    void equals();
    void backspace();
    void clear();
    bool calculate(double left, double right, const QString &op, double &result);
    static QString number(double value);

    QString m_display = "0";
    QString m_expression;
    QString m_operator;
    double m_left = 0;
    bool m_startNewNumber = true;
    bool m_hasLeft = false;
    bool m_afterEquals = false;
    bool m_error = false;
};

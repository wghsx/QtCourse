#include "calculatorengine.h"

#include <QtMath>

void CalculatorEngine::input(const QString &key)
{
    if (key == "C") { clear(); return; }
    if (m_error) {
        if (key.size() == 1 && key[0].isDigit()) clear();
        else if (key == ".") clear();
        else return;
    }
    if (key.size() == 1 && key[0].isDigit()) digit(key);
    else if (key == ".") decimalPoint();
    else if (key == "⌫") backspace();
    else if (key == "+" || key == "-" || key == "×" || key == "÷") operation(key);
    else if (key == "=") equals();
}

void CalculatorEngine::digit(const QString &value)
{
    if (m_afterEquals) { m_expression.clear(); m_afterEquals = false; }
    if (m_startNewNumber || m_display == "0") m_display = value;
    else if (m_display.size() < 16) m_display += value;
    m_startNewNumber = false;
}

void CalculatorEngine::decimalPoint()
{
    if (m_afterEquals) { m_expression.clear(); m_afterEquals = false; }
    if (m_startNewNumber) { m_display = "0."; m_startNewNumber = false; }
    else if (!m_display.contains('.')) m_display += '.';
}

void CalculatorEngine::operation(const QString &value)
{
    if (m_hasLeft && !m_startNewNumber) {
        double result = 0;
        if (!calculate(m_left, m_display.toDouble(), m_operator, result)) return;
        m_left = result;
        m_display = number(result);
    } else if (!m_hasLeft) {
        m_left = m_display.toDouble();
        m_hasLeft = true;
    }
    // Repeated operators replace the pending operator instead of inventing an operand.
    m_operator = value;
    m_expression = number(m_left) + " " + value;
    m_startNewNumber = true;
    m_afterEquals = false;
}

void CalculatorEngine::equals()
{
    if (!m_hasLeft || m_startNewNumber) return;
    const QString right = m_display;
    double result = 0;
    if (!calculate(m_left, right.toDouble(), m_operator, result)) return;
    m_expression = number(m_left) + " " + m_operator + " " + right + " =";
    m_display = number(result);
    m_operator.clear();
    m_hasLeft = false;
    m_startNewNumber = true;
    m_afterEquals = true;
}

void CalculatorEngine::backspace()
{
    if (m_afterEquals) { m_expression.clear(); m_afterEquals = false; }
    if (m_startNewNumber) return;
    m_display.chop(1);
    if (m_display.isEmpty() || m_display == "-") m_display = "0";
}

void CalculatorEngine::clear()
{
    m_display = "0";
    m_expression.clear();
    m_operator.clear();
    m_left = 0;
    m_startNewNumber = true;
    m_hasLeft = false;
    m_afterEquals = false;
    m_error = false;
}

bool CalculatorEngine::calculate(double left, double right, const QString &op, double &result)
{
    if (op == "+") result = left + right;
    else if (op == "-") result = left - right;
    else if (op == "×") result = left * right;
    else if (op == "÷") {
        if (right == 0) {
            m_expression = number(left) + " ÷ " + number(right) + " =";
            m_display = "不能除以零";
            m_error = true;
            m_hasLeft = false;
            m_operator.clear();
            m_startNewNumber = true;
            return false;
        }
        result = left / right;
    } else return false;
    if (!qIsFinite(result)) {
        m_display = "结果超出范围";
        m_error = true;
        m_hasLeft = false;
        m_operator.clear();
        m_startNewNumber = true;
        return false;
    }
    return true;
}

QString CalculatorEngine::number(double value)
{
    if (qAbs(value) < 1e-14) value = 0;
    return QString::number(value, 'g', 15);
}

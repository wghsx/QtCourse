#pragma once

#include <QWidget>
#include "calculatorengine.h"

namespace Ui { class CalculatorWindow; }

class CalculatorWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CalculatorWindow(QWidget *parent = nullptr);
    ~CalculatorWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void dispatch(const QString &key);
    Ui::CalculatorWindow *ui;
    CalculatorEngine m_engine;
};

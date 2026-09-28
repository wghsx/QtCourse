# 带键盘事件的计算器

Qt Widgets / qmake 工程。用 Qt Creator 打开 `Calculator.pro`，选择 Qt 6.8.3 MinGW 64-bit 套件后构建并运行。界面在 `calculatorwindow.ui` 中，可用 Qt Designer 编辑。

## 操作

鼠标点击按钮，或使用键盘 `0`–`9`、`.`、`+`、`-`、`*`、`/`、`Enter`/`=`、`Backspace`、`Esc`/`Delete`。`x` 也可输入乘法。键盘和按钮均进入 `CalculatorWindow::dispatch()`，再由 `CalculatorEngine::input()` 处理。

## 设计

`CalculatorEngine` 保存当前显示值、第一个操作数、待执行操作符、是否等待新数字、是否刚完成计算和错误状态。连续操作符替换前一个操作符；输入第二个数后选择新操作符会先计算前一步。除零显示“不能除以零”，之后输入数字或清除可恢复。计算结果后直接输入数字开始新计算；选择操作符则以结果为第一个操作数。

## 验证

在 `tests` 目录用 qmake 构建 `tests.pro`，运行 `CalculatorTests.exe -o results.txt,txt`。自动测试覆盖四则运算、连续运算、重复小数点和操作符、除零、退格、清除、计算后输入及鼠标与键盘混合输入。`calculator-screenshot.png` 是 Windows 图形环境中由测试程序截取的实际 Qt 窗口。

本机验证环境为 Qt 6.8.3 + MinGW 13.1.0，实验模板所列 Qt 6.9.2 未在本机安装。

`实验1-带键盘事件的计算器-演示视频.mp4` 为 2 分 05 秒的实际 Qt 窗口录制。演示操作由 `tests/demo.cpp` 中的 QtTest 鼠标和键盘事件驱动，末尾展示真实 `git log`。视频可直接作为作业系统附件提交。

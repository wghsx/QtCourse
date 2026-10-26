# 小牛文本编辑器（简易多窗口文本编辑器）

Qt Widgets 课程实验二：使用 Qt Designer（.ui）+ Action 编辑器 + 自定义控件完成一个多窗口文本编辑器。

## 运行环境

- Windows 64 位
- Qt 6.8.3 (MinGW 13.1.0 64-bit)，qmake + mingw32-make
- Git

## 构建

```
qmake TextEditor.pro
mingw32-make -j4
```

或直接使用 Qt Creator 打开 `TextEditor.pro` 后构建运行。

## 功能

- 文件：新建 / 打开 / 保存 / 另存为 / 退出，根据修改状态自动更新窗口标题（未保存时标题前带 `*`）
- 编辑：撤销 / 恢复 / 剪切 / 复制 / 粘贴 / 全选，Action 使能状态随编辑状态变化
- 查找 / 替换：查找下一个、替换、全部替换，支持区分大小写与方向（向上/向下），支持“未找到”与连续（循环）查找
- 格式：自动换行、显示行号、字体、字体颜色、字体背景色、编辑器背景色、定时自动保存（30 秒，选做）
- 查看：显示/隐藏工具栏、状态栏
- 帮助：关于对话框
- 行号：`CodeEditor` 继承 `QPlainTextEdit`，通过 UI 中的 promote（提升为…）替换默认 `QPlainTextEdit`

## 目录结构

```
TextEditor.pro          工程文件
main.cpp                程序入口（浅色主题、中文翻译）
mainwindow.ui/.h/.cpp   主窗口界面与逻辑
codeeditor.h/.cpp       带行号的编辑器控件（promote 使用）
finddialog.*            查找对话框
replacedialog.*         替换对话框
aboutdialog.*           关于对话框
resources.qrc/icons     工具栏与菜单图标
testdata/demo.txt       测试用文本
```

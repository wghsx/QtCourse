#include "mainwindow.h"

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyleFactory>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 统一使用浅色主题，保证界面在不同系统主题下显示一致。
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0xF0, 0xF0, 0xF0));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(0xE9, 0xE9, 0xE9));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(0xF0, 0xF0, 0xF0));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(0x2B, 0x57, 0x9A));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(0x9A, 0x9A, 0x9A));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x9A, 0x9A, 0x9A));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x9A, 0x9A, 0x9A));
    app.setPalette(palette);

    // 载入 Qt 内置控件的中文翻译（文件对话框等）
    QTranslator translator;
    const QStringList qmDirs = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/translations"),
        QLibraryInfo::path(QLibraryInfo::TranslationsPath)
    };
    for (const QString &dir : qmDirs) {
        if (translator.load(QStringLiteral("qtbase_zh_CN"), dir)) {
            app.installTranslator(&translator);
            break;
        }
    }

    MainWindow window;
    window.show();
    return app.exec();
}

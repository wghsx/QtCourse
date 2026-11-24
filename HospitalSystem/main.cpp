#include "model/databasemanager.h"
#include "view/mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("医院诊疗信息管理系统"));
    QApplication::setOrganizationName(QStringLiteral("DGUT"));
    QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QFile styleFile(QStringLiteral(":/style.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    // 允许用环境变量指定数据库文件，便于演示“连接失败”等异常场景。
    const QString databasePath = qEnvironmentVariable(
        "HOSPITAL_DB", DatabaseManager::defaultDatabasePath());

    QString error;
    if (!DatabaseManager::instance().open(databasePath, &error)) {
        QString reason = error;
        if (reason.contains(QStringLiteral("unable to open database file")))
            reason = QStringLiteral("无法打开数据库文件，路径不存在或当前用户没有读写权限。");
        QMessageBox box(QMessageBox::Critical, QStringLiteral("数据库连接失败"),
                        QStringLiteral("无法连接数据库：\n%1").arg(databasePath),
                        QMessageBox::Ok);
        box.setInformativeText(QStringLiteral("原因：%1\n\n请检查文件路径和读写权限后重新启动程序。")
                                   .arg(reason));
        box.setMinimumWidth(560);
        box.exec();
        return 1;
    }

    if (!DatabaseManager::instance().createSchema(&error)
        || !DatabaseManager::instance().seedDefaultData(&error)) {
        QMessageBox box(QMessageBox::Critical, QStringLiteral("数据库初始化失败"),
                        QStringLiteral("建表或写入初始数据时出错。"), QMessageBox::Ok);
        box.setInformativeText(error);
        box.setMinimumWidth(560);
        box.exec();
        return 1;
    }

    MainWindow window;
    window.show();
    return app.exec();
}

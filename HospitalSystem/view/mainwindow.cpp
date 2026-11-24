#include "mainwindow.h"

#include "departmentpage.h"
#include "doctorpage.h"
#include "historydialog.h"
#include "loginpage.h"
#include "model/databasemanager.h"
#include "patientpage.h"
#include "welcomepage.h"

#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("医院诊疗信息管理系统"));
    resize(1120, 720);
    setMinimumSize(980, 640);

    buildUi();
    switchTo(LoginPageIndex);
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    central->setObjectName(QStringLiteral("centralArea"));
    setCentralWidget(central);

    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *topBar = new QFrame(central);
    topBar->setObjectName(QStringLiteral("topBar"));
    topBar->setFixedHeight(58);

    auto *barLayout = new QHBoxLayout(topBar);
    barLayout->setContentsMargins(16, 0, 18, 0);
    barLayout->setSpacing(10);

    m_backButton = new QToolButton(topBar);
    m_backButton->setObjectName(QStringLiteral("backButton"));
    m_backButton->setText(QStringLiteral("‹"));
    m_backButton->setToolTip(QStringLiteral("返回"));
    m_backButton->setCursor(Qt::PointingHandCursor);
    m_backButton->setFixedSize(34, 34);

    auto *brandLabel = new QLabel(QStringLiteral("医院诊疗系统"), topBar);
    brandLabel->setObjectName(QStringLiteral("brandLabel"));

    m_titleLabel = new QLabel(topBar);
    m_titleLabel->setObjectName(QStringLiteral("titleLabel"));
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_userLabel = new QLabel(topBar);
    m_userLabel->setObjectName(QStringLiteral("userLabel"));

    m_logoutButton = new QToolButton(topBar);
    m_logoutButton->setObjectName(QStringLiteral("logoutButton"));
    m_logoutButton->setText(QStringLiteral("退出登录"));
    m_logoutButton->setCursor(Qt::PointingHandCursor);

    barLayout->addWidget(m_backButton);
    barLayout->addWidget(brandLabel);
    barLayout->addSpacing(8);
    barLayout->addWidget(m_titleLabel, 1);
    barLayout->addWidget(m_userLabel);
    barLayout->addWidget(m_logoutButton);
    root->addWidget(topBar);

    m_stack = new QStackedWidget(central);
    m_stack->setObjectName(QStringLiteral("mainStack"));

    m_loginPage = new LoginPage(m_stack);
    m_welcomePage = new WelcomePage(m_stack);
    m_departmentPage = new DepartmentPage(m_stack);
    m_doctorPage = new DoctorPage(m_stack);
    m_patientPage = new PatientPage(m_stack);

    m_stack->addWidget(m_loginPage);
    m_stack->addWidget(m_welcomePage);
    m_stack->addWidget(m_departmentPage);
    m_stack->addWidget(m_doctorPage);
    m_stack->addWidget(m_patientPage);
    root->addWidget(m_stack, 1);

    auto *developerLabel = new QLabel(QStringLiteral("开发者：黄圣杰　学号：2024414290414"), this);
    developerLabel->setObjectName(QStringLiteral("statusLabel"));
    statusBar()->addWidget(developerLabel);

    const QString databaseFile =
        QFileInfo(DatabaseManager::instance().database().databaseName()).fileName();
    auto *databaseLabel = new QLabel(
        QStringLiteral("SQLite 已连接：%1").arg(databaseFile), this);
    databaseLabel->setObjectName(QStringLiteral("statusLabel"));
    databaseLabel->setToolTip(DatabaseManager::instance().database().databaseName());
    statusBar()->addPermanentWidget(databaseLabel);
    statusBar()->setSizeGripEnabled(false);

    connect(m_backButton, &QToolButton::clicked, this, &MainWindow::goBack);
    connect(m_logoutButton, &QToolButton::clicked, this, &MainWindow::logout);
    connect(m_loginPage, &LoginPage::loginSucceeded, this, &MainWindow::handleLogin);
    connect(m_welcomePage, &WelcomePage::departmentRequested, this,
            [this] { switchTo(DepartmentPageIndex); });
    connect(m_welcomePage, &WelcomePage::doctorRequested, this,
            [this] { switchTo(DoctorPageIndex); });
    connect(m_welcomePage, &WelcomePage::patientRequested, this,
            [this] { switchTo(PatientPageIndex); });
    connect(m_welcomePage, &WelcomePage::historyRequested, this, &MainWindow::showHistory);
}

void MainWindow::handleLogin(const QString &userId, const QString &fullName,
                             const QString &username)
{
    m_userId = userId;
    m_fullName = fullName;
    m_username = username;

    m_patientPage->setCurrentUser(m_userId, m_fullName);
    m_welcomePage->setCurrentUser(m_fullName);
    updateUserArea();
    switchTo(WelcomePageIndex);
}

void MainWindow::updateUserArea()
{
    m_userLabel->setText(QStringLiteral("%1（%2）").arg(m_fullName, m_username));
}

void MainWindow::switchTo(PageIndex page)
{
    m_stack->setCurrentIndex(page);

    switch (page) {
    case LoginPageIndex:
        m_titleLabel->setText(QStringLiteral("登录"));
        m_backButton->setVisible(false);
        m_userLabel->setVisible(false);
        m_logoutButton->setVisible(false);
        break;
    case WelcomePageIndex:
        m_titleLabel->setText(QStringLiteral("欢迎"));
        m_backButton->setVisible(false);
        m_userLabel->setVisible(true);
        m_logoutButton->setVisible(true);
        break;
    case DepartmentPageIndex:
        m_titleLabel->setText(QStringLiteral("科室管理"));
        m_backButton->setVisible(true);
        m_departmentPage->refresh();
        break;
    case DoctorPageIndex:
        m_titleLabel->setText(QStringLiteral("医生管理"));
        m_backButton->setVisible(true);
        m_doctorPage->refresh();
        break;
    case PatientPageIndex:
        m_titleLabel->setText(QStringLiteral("患者管理"));
        m_backButton->setVisible(true);
        m_patientPage->refresh();
        m_patientPage->setFocus();
        break;
    }
}

void MainWindow::goBack()
{
    if (m_stack->currentIndex() != LoginPageIndex
        && m_stack->currentIndex() != WelcomePageIndex) {
        switchTo(WelcomePageIndex);
    }
}

void MainWindow::logout()
{
    QMessageBox box(this);
    box.setWindowTitle(QStringLiteral("退出登录"));
    box.setIcon(QMessageBox::Question);
    box.setText(QStringLiteral("确定退出当前账号吗？"));
    QPushButton *confirmButton = box.addButton(QStringLiteral("退出"), QMessageBox::AcceptRole);
    box.addButton(QStringLiteral("取消"), QMessageBox::RejectRole);
    box.setDefaultButton(nullptr);
    box.exec();
    if (box.clickedButton() != confirmButton)
        return;

    m_userId.clear();
    m_fullName.clear();
    m_username.clear();
    m_userLabel->clear();
    m_loginPage->reset();
    switchTo(LoginPageIndex);
}

void MainWindow::showHistory()
{
    HistoryDialog dialog(this);
    dialog.exec();
}

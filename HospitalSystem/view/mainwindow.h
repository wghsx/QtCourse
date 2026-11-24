#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QStackedWidget;
class QToolButton;

class LoginPage;
class WelcomePage;
class DepartmentPage;
class DoctorPage;
class PatientPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void handleLogin(const QString &userId, const QString &fullName, const QString &username);
    void goBack();
    void logout();
    void showHistory();

private:
    enum PageIndex {
        LoginPageIndex = 0,
        WelcomePageIndex = 1,
        DepartmentPageIndex = 2,
        DoctorPageIndex = 3,
        PatientPageIndex = 4
    };

    void buildUi();
    void switchTo(PageIndex page);
    void updateUserArea();

    QStackedWidget *m_stack = nullptr;
    QToolButton *m_backButton = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_userLabel = nullptr;
    QToolButton *m_logoutButton = nullptr;

    LoginPage *m_loginPage = nullptr;
    WelcomePage *m_welcomePage = nullptr;
    DepartmentPage *m_departmentPage = nullptr;
    DoctorPage *m_doctorPage = nullptr;
    PatientPage *m_patientPage = nullptr;

    QString m_userId;
    QString m_fullName;
    QString m_username;
};

#endif // MAINWINDOW_H

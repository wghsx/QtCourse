#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QFrame>
#include <QWidget>

class QLabel;

// 首页的模块入口卡片，整块可点击。
class ModuleCard : public QFrame
{
    Q_OBJECT
public:
    ModuleCard(const QString &glyph, const QString &title, const QString &subtitle,
               QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setHovered(bool hovered);
};

class WelcomePage : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomePage(QWidget *parent = nullptr);

    void setCurrentUser(const QString &fullName);

signals:
    void departmentRequested();
    void doctorRequested();
    void patientRequested();
    void historyRequested();

private:
    QLabel *m_greetingLabel = nullptr;
};

#endif // WELCOMEPAGE_H

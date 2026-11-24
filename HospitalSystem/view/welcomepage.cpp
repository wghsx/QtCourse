#include "welcomepage.h"

#include <QEnterEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

ModuleCard::ModuleCard(const QString &glyph, const QString &title, const QString &subtitle,
                       QWidget *parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("moduleCard"));
    setProperty("hovered", false);
    setCursor(Qt::PointingHandCursor);
    setFixedSize(230, 186);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 26, 24, 24);
    layout->setSpacing(8);

    auto *glyphLabel = new QLabel(glyph, this);
    glyphLabel->setObjectName(QStringLiteral("moduleGlyph"));
    glyphLabel->setAlignment(Qt::AlignCenter);
    glyphLabel->setFixedSize(48, 48);
    layout->addWidget(glyphLabel, 0, Qt::AlignLeft);

    auto *titleLabel = new QLabel(title, this);
    titleLabel->setObjectName(QStringLiteral("moduleTitle"));
    layout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel(subtitle, this);
    subtitleLabel->setObjectName(QStringLiteral("moduleSubtitle"));
    subtitleLabel->setWordWrap(true);
    layout->addWidget(subtitleLabel);

    layout->addStretch();
}

void ModuleCard::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->position().toPoint()))
        emit clicked();
    QFrame::mouseReleaseEvent(event);
}

void ModuleCard::enterEvent(QEnterEvent *event)
{
    setHovered(true);
    QFrame::enterEvent(event);
}

void ModuleCard::leaveEvent(QEvent *event)
{
    setHovered(false);
    QFrame::leaveEvent(event);
}

void ModuleCard::setHovered(bool hovered)
{
    setProperty("hovered", hovered);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(56, 44, 56, 40);
    root->setSpacing(0);

    auto *title = new QLabel(QStringLiteral("欢迎使用诊疗测试系统"), this);
    title->setObjectName(QStringLiteral("pageHeading"));
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    m_greetingLabel = new QLabel(QStringLiteral("请选择要进入的管理模块"), this);
    m_greetingLabel->setObjectName(QStringLiteral("pageSubheading"));
    m_greetingLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_greetingLabel);

    root->addSpacing(36);

    auto *cards = new QHBoxLayout;
    cards->setSpacing(24);
    cards->addStretch();

    auto *departmentCard = new ModuleCard(QStringLiteral("科"), QStringLiteral("科室管理"),
                                          QStringLiteral("查看科室列表与各科医生人数"), this);
    auto *doctorCard = new ModuleCard(QStringLiteral("医"), QStringLiteral("医生管理"),
                                      QStringLiteral("查看医生工号与所属科室"), this);
    auto *patientCard = new ModuleCard(QStringLiteral("患"), QStringLiteral("患者管理"),
                                       QStringLiteral("查询、新增、修改与删除患者档案"), this);
    cards->addWidget(departmentCard);
    cards->addWidget(doctorCard);
    cards->addWidget(patientCard);
    cards->addStretch();
    root->addLayout(cards);

    root->addSpacing(30);

    auto *historyButton = new QPushButton(QStringLiteral("查看操作记录"), this);
    historyButton->setObjectName(QStringLiteral("linkButton"));
    historyButton->setCursor(Qt::PointingHandCursor);
    auto *historyRow = new QHBoxLayout;
    historyRow->addStretch();
    historyRow->addWidget(historyButton);
    historyRow->addStretch();
    root->addLayout(historyRow);

    root->addStretch();

    connect(departmentCard, &ModuleCard::clicked, this, &WelcomePage::departmentRequested);
    connect(doctorCard, &ModuleCard::clicked, this, &WelcomePage::doctorRequested);
    connect(patientCard, &ModuleCard::clicked, this, &WelcomePage::patientRequested);
    connect(historyButton, &QPushButton::clicked, this, &WelcomePage::historyRequested);
}

void WelcomePage::setCurrentUser(const QString &fullName)
{
    m_greetingLabel->setText(
        QStringLiteral("当前登录：%1　请选择要进入的管理模块").arg(fullName));
}

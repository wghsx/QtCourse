#include "departmentpage.h"

#include "model/databasemanager.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

DepartmentPage::DepartmentPage(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 18, 24, 16);
    root->setSpacing(12);

    auto *tools = new QHBoxLayout;
    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setObjectName(QStringLiteral("summaryLabel"));
    auto *refreshButton = new QPushButton(QStringLiteral("刷新"), this);
    tools->addWidget(m_summaryLabel);
    tools->addStretch();
    tools->addWidget(refreshButton);
    root->addLayout(tools);

    m_table = new QTableView(this);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(34);
    m_table->horizontalHeader()->setStretchLastSection(true);
    root->addWidget(m_table, 1);

    m_model = new QSqlQueryModel(this);
    m_model->setQuery(QStringLiteral(
        "SELECT d.ID AS \"科室编号\", d.NAME AS \"科室名称\","
        " COUNT(dc.ID) AS \"医生人数\""
        " FROM Department d LEFT JOIN Doctor dc ON dc.DEPARTMENT_ID = d.ID"
        " GROUP BY d.ID, d.NAME ORDER BY d.ID"),
        DatabaseManager::instance().database());
    m_table->setModel(m_model);
    m_table->setColumnWidth(0, 140);
    m_table->setColumnWidth(1, 220);

    m_summaryLabel->setText(QStringLiteral("共 %1 个科室").arg(m_model->rowCount()));

    connect(refreshButton, &QPushButton::clicked, this, &DepartmentPage::refresh);
}

void DepartmentPage::refresh()
{
    m_model->setQuery(m_model->query().lastQuery(), DatabaseManager::instance().database());
    while (m_model->canFetchMore())
        m_model->fetchMore();
    m_summaryLabel->setText(QStringLiteral("共 %1 个科室").arg(m_model->rowCount()));
}

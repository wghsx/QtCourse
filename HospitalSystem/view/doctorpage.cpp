#include "doctorpage.h"

#include "model/databasemanager.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

DoctorPage::DoctorPage(QWidget *parent)
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
        "SELECT d.ID AS \"医生编号\", d.EMPLOYEENO AS \"工号\", d.NAME AS \"姓名\","
        " dep.NAME AS \"所属科室\""
        " FROM Doctor d LEFT JOIN Department dep ON dep.ID = d.DEPARTMENT_ID"
        " ORDER BY d.ID"),
        DatabaseManager::instance().database());
    m_table->setModel(m_model);
    m_table->setColumnWidth(0, 120);
    m_table->setColumnWidth(1, 140);
    m_table->setColumnWidth(2, 180);

    m_summaryLabel->setText(QStringLiteral("共 %1 位医生").arg(m_model->rowCount()));

    connect(refreshButton, &QPushButton::clicked, this, &DoctorPage::refresh);
}

void DoctorPage::refresh()
{
    m_model->setQuery(m_model->query().lastQuery(), DatabaseManager::instance().database());
    while (m_model->canFetchMore())
        m_model->fetchMore();
    m_summaryLabel->setText(QStringLiteral("共 %1 位医生").arg(m_model->rowCount()));
}

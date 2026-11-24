#include "historydialog.h"

#include "model/databasemanager.h"

#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

HistoryDialog::HistoryDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("操作记录"));
    setModal(true);
    resize(720, 460);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(20, 18, 20, 16);
    root->setSpacing(10);

    auto *hint = new QLabel(QStringLiteral("最近 200 条登录与数据变更记录"), this);
    hint->setObjectName(QStringLiteral("mutedLabel"));
    root->addWidget(hint);

    m_table = new QTableView(this);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(30);
    m_table->horizontalHeader()->setStretchLastSection(true);
    root->addWidget(m_table, 1);

    m_model = new QSqlQueryModel(this);
    m_model->setQuery(QStringLiteral(
        "SELECT h.TIMESTAMP AS \"时间\", IFNULL(u.FULLNAME, \"未登录\") AS \"操作人\","
        " h.EVENT AS \"操作内容\""
        " FROM History h LEFT JOIN \"User\" u ON u.ID = h.USER_ID"
        " ORDER BY h.ID DESC LIMIT 200"),
        DatabaseManager::instance().database());
    m_table->setModel(m_model);
    m_table->setColumnWidth(0, 160);
    m_table->setColumnWidth(1, 120);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttons->button(QDialogButtonBox::Close)->setText(QStringLiteral("关闭"));
    root->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::rejected, this, &HistoryDialog::reject);
}

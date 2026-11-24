#include "patientpage.h"

#include "model/databasemanager.h"
#include "model/historyrepository.h"
#include "model/patientrepository.h"
#include "patientdialog.h"
#include "sexdelegate.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>

PatientPage::PatientPage(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 18, 24, 16);
    root->setSpacing(12);

    auto *tools = new QHBoxLayout;
    tools->setSpacing(8);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(QStringLiteral("按姓名 / 病历号 / 身份证号 / 手机号查询"));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumWidth(340);

    auto *searchButton = new QPushButton(QStringLiteral("查询"), this);
    searchButton->setObjectName(QStringLiteral("primaryButton"));
    auto *resetButton = new QPushButton(QStringLiteral("重置"), this);

    m_addButton = new QPushButton(QStringLiteral("添加"), this);
    m_editButton = new QPushButton(QStringLiteral("修改"), this);
    m_deleteButton = new QPushButton(QStringLiteral("删除"), this);
    m_deleteButton->setObjectName(QStringLiteral("dangerButton"));

    tools->addWidget(m_searchEdit);
    tools->addWidget(searchButton);
    tools->addWidget(resetButton);
    tools->addStretch();
    tools->addWidget(m_addButton);
    tools->addWidget(m_editButton);
    tools->addWidget(m_deleteButton);
    root->addLayout(tools);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setObjectName(QStringLiteral("summaryLabel"));
    root->addWidget(m_summaryLabel);

    m_table = new QTableView(this);
    m_table->setObjectName(QStringLiteral("patientTable"));
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setSortingEnabled(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(34);
    m_table->horizontalHeader()->setHighlightSections(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    root->addWidget(m_table, 1);

    setupModel();

    connect(searchButton, &QPushButton::clicked, this, &PatientPage::applyFilter);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &PatientPage::applyFilter);
    connect(resetButton, &QPushButton::clicked, this, &PatientPage::clearFilter);
    connect(m_addButton, &QPushButton::clicked, this, &PatientPage::addPatient);
    connect(m_editButton, &QPushButton::clicked, this, &PatientPage::editPatient);
    connect(m_deleteButton, &QPushButton::clicked, this, &PatientPage::deletePatient);
    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PatientPage::updateActions);

    // 双击“性别”列直接在表格内下拉修改，其余列进入修改对话框。
    connect(m_table, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        if (index.column() == 3)
            m_table->edit(index);
        else
            editPatient();
    });

    updateActions();
    updateSummary(QString());
}

void PatientPage::setCurrentUser(const QString &userId, const QString &fullName)
{
    m_userId = userId;
    m_userName = fullName;
}

void PatientPage::setupModel()
{
    m_model = new QSqlTableModel(this, DatabaseManager::instance().database());
    m_model->setTable(QStringLiteral("Patient"));
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    m_model->setHeaderData(0, Qt::Horizontal, QStringLiteral("病历号"));
    m_model->setHeaderData(1, Qt::Horizontal, QStringLiteral("身份证号"));
    m_model->setHeaderData(2, Qt::Horizontal, QStringLiteral("姓名"));
    m_model->setHeaderData(3, Qt::Horizontal, QStringLiteral("性别"));
    m_model->setHeaderData(4, Qt::Horizontal, QStringLiteral("出生日期"));
    m_model->setHeaderData(5, Qt::Horizontal, QStringLiteral("身高(cm)"));
    m_model->setHeaderData(6, Qt::Horizontal, QStringLiteral("体重(kg)"));
    m_model->setHeaderData(7, Qt::Horizontal, QStringLiteral("手机号"));
    m_model->setHeaderData(8, Qt::Horizontal, QStringLiteral("年龄"));
    m_model->setHeaderData(9, Qt::Horizontal, QStringLiteral("建档时间"));

    m_model->select();

    m_table->setModel(m_model);
    m_table->setItemDelegateForColumn(3, new SexDelegate(this));

    m_table->setColumnWidth(0, 140);
    m_table->setColumnWidth(1, 158);
    m_table->setColumnWidth(2, 76);
    m_table->setColumnWidth(3, 56);
    m_table->setColumnWidth(4, 100);
    m_table->setColumnWidth(5, 74);
    m_table->setColumnWidth(6, 74);
    m_table->setColumnWidth(7, 118);
    m_table->setColumnWidth(8, 54);
    m_table->setColumnWidth(9, 140);

    connect(m_model, &QSqlTableModel::dataChanged,
            this, &PatientPage::submitInlineEdit);
}

void PatientPage::refresh()
{
    m_model->select();
    while (m_model->canFetchMore())
        m_model->fetchMore();
    updateSummary(m_searchEdit->text().trimmed());
    updateActions();
}

void PatientPage::applyFilter()
{
    const QString keyword = m_searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        m_model->setFilter(QString());
    } else {
        QString escaped = keyword;
        escaped.replace(QLatin1Char('\''), QStringLiteral("''"));
        m_model->setFilter(QStringLiteral(
            "NAME LIKE '%%1%' OR ID LIKE '%%1%' OR ID_CARD LIKE '%%1%'"
            " OR MOBILEPHONE LIKE '%%1%'").arg(escaped));
    }
    m_model->select();
    while (m_model->canFetchMore())
        m_model->fetchMore();
    updateSummary(keyword);
    updateActions();
}

void PatientPage::clearFilter()
{
    m_searchEdit->clear();
    m_model->setFilter(QString());
    refresh();
}

void PatientPage::updateSummary(const QString &keyword)
{
    const int rows = m_model->rowCount();
    if (!keyword.isEmpty() && rows == 0) {
        m_summaryLabel->setText(
            QStringLiteral("未找到符合条件的记录（关键字：%1），请调整查询条件。").arg(keyword));
        m_summaryLabel->setStyleSheet(QStringLiteral("color: #b26a00;"));
        return;
    }
    m_summaryLabel->setText(QStringLiteral("共 %1 条患者记录").arg(rows));
    m_summaryLabel->setStyleSheet(QStringLiteral("color: #5b6b80;"));
}

QModelIndex PatientPage::currentRow() const
{
    if (!m_table->selectionModel())
        return QModelIndex();
    const QModelIndexList rows = m_table->selectionModel()->selectedRows();
    return rows.isEmpty() ? QModelIndex() : rows.first();
}

void PatientPage::updateActions()
{
    const bool hasSelection = currentRow().isValid();
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

void PatientPage::addPatient()
{
    PatientDialog dialog(PatientDialog::AddMode, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString id = dialog.record().id;
    HistoryRepository::record(m_userId, QStringLiteral("新增患者 %1").arg(id));
    m_searchEdit->clear();
    m_model->setFilter(QString());
    refresh();

    const int row = m_model->match(m_model->index(0, 0), Qt::EditRole, id).value(0).row();
    if (row >= 0) {
        m_table->selectRow(row);
        m_table->scrollTo(m_model->index(row, 0));
    }
}

void PatientPage::editPatient()
{
    const QModelIndex index = currentRow();
    if (!index.isValid()) {
        QMessageBox::information(this, QStringLiteral("修改"), QStringLiteral("请先选择一条记录。"));
        return;
    }

    const QSqlRecord sqlRecord = m_model->record(index.row());
    PatientRecord data;
    data.id = sqlRecord.value(QStringLiteral("ID")).toString();
    data.idCard = sqlRecord.value(QStringLiteral("ID_CARD")).toString();
    data.name = sqlRecord.value(QStringLiteral("NAME")).toString();
    data.sex = sqlRecord.value(QStringLiteral("SEX")).toInt();
    data.dob = sqlRecord.value(QStringLiteral("DOB")).toString();
    data.height = sqlRecord.value(QStringLiteral("HEIGHT")).toDouble();
    data.weight = sqlRecord.value(QStringLiteral("WEIGHT")).toDouble();
    data.mobile = sqlRecord.value(QStringLiteral("MOBILEPHONE")).toString();
    data.age = sqlRecord.value(QStringLiteral("AGE")).toInt();
    data.createdTimestamp = sqlRecord.value(QStringLiteral("CREATEDTIMESTAMP")).toString();

    PatientDialog dialog(PatientDialog::EditMode, this);
    dialog.setRecord(data);
    if (dialog.exec() != QDialog::Accepted)
        return;

    HistoryRepository::record(m_userId, QStringLiteral("修改患者 %1").arg(data.id));
    const QString id = data.id;
    refresh();
    const int row = m_model->match(m_model->index(0, 0), Qt::EditRole, id).value(0).row();
    if (row >= 0) {
        m_table->selectRow(row);
        m_table->scrollTo(m_model->index(row, 0));
    }
}

void PatientPage::deletePatient()
{
    const QModelIndex index = currentRow();
    if (!index.isValid()) {
        QMessageBox::information(this, QStringLiteral("删除"), QStringLiteral("请先选择一条记录。"));
        return;
    }

    const QSqlRecord sqlRecord = m_model->record(index.row());
    const QString id = sqlRecord.value(QStringLiteral("ID")).toString();
    const QString name = sqlRecord.value(QStringLiteral("NAME")).toString();
    const QString idCard = sqlRecord.value(QStringLiteral("ID_CARD")).toString();

    QMessageBox box(this);
    box.setWindowTitle(QStringLiteral("删除确认"));
    box.setIcon(QMessageBox::Question);
    box.setText(QStringLiteral("确定删除患者「%1」吗？").arg(name));
    box.setInformativeText(QStringLiteral("病历号：%1\n身份证号：%2\n\n该记录删除后不可恢复。")
                               .arg(id, idCard));
    QPushButton *confirmButton = box.addButton(QStringLiteral("删除"), QMessageBox::DestructiveRole);
    box.addButton(QStringLiteral("取消"), QMessageBox::RejectRole);
    box.setDefaultButton(nullptr);
    box.setMinimumWidth(400);
    box.exec();
    if (box.clickedButton() != confirmButton)
        return;

    QString error;
    if (!PatientRepository::remove(id, &error)) {
        QMessageBox::warning(this, QStringLiteral("删除失败"), error);
        return;
    }

    HistoryRepository::record(m_userId, QStringLiteral("删除患者 %1").arg(id));
    refresh();
    QMessageBox done(QMessageBox::Information, QStringLiteral("删除完成"),
                     QStringLiteral("患者「%1」已删除，当前剩余 %2 条记录。")
                         .arg(name).arg(m_model->rowCount()),
                     QMessageBox::Ok, this);
    done.button(QMessageBox::Ok)->setText(QStringLiteral("确定"));
    done.exec();
}

void PatientPage::submitInlineEdit()
{
    if (m_submitting)
        return;
    m_submitting = true;
    if (!m_model->submitAll()) {
        const QString error = m_model->lastError().text();
        m_model->revertAll();
        QMessageBox::warning(this, QStringLiteral("保存失败"),
                             QStringLiteral("表格中的修改未写入数据库：\n%1").arg(error));
    } else {
        HistoryRepository::record(m_userId, QStringLiteral("表格内修改患者性别"));
    }
    m_submitting = false;
    updateSummary(m_searchEdit->text().trimmed());
}

#include "patientdialog.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QKeySequence>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QVBoxLayout>

PatientDialog::PatientDialog(Mode mode, QWidget *parent)
    : QDialog(parent)
    , m_mode(mode)
{
    setWindowTitle(mode == AddMode ? QStringLiteral("新增患者") : QStringLiteral("修改患者信息"));
    setModal(true);
    setMinimumWidth(500);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 20);
    root->setSpacing(14);

    auto *title = new QLabel(mode == AddMode ? QStringLiteral("新增患者档案")
                                             : QStringLiteral("修改患者档案"), this);
    title->setObjectName(QStringLiteral("dialogTitle"));
    root->addWidget(title);

    auto *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(10);

    m_idEdit = new QLineEdit(this);
    m_idEdit->setMaxLength(32);
    if (mode == AddMode) {
        m_idEdit->setText(QStringLiteral("P%1").arg(
            QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMddhhmmss"))));
    } else {
        m_idEdit->setReadOnly(true);
        m_idEdit->setStyleSheet(QStringLiteral("background:#f4f6fa;color:#6b7c95;"));
    }

    m_idCardEdit = new QLineEdit(this);
    m_idCardEdit->setMaxLength(18);
    m_idCardEdit->setPlaceholderText(QStringLiteral("18 位身份证号"));

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setMaxLength(20);

    m_sexBox = new QComboBox(this);
    m_sexBox->addItem(QStringLiteral("男"), 1);
    m_sexBox->addItem(QStringLiteral("女"), 0);

    m_dobEdit = new QDateEdit(QDate(1990, 1, 1), this);
    m_dobEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m_dobEdit->setCalendarPopup(true);
    m_dobEdit->setMaximumDate(QDate::currentDate());

    m_heightBox = new QDoubleSpinBox(this);
    m_heightBox->setRange(30.0, 250.0);
    m_heightBox->setDecimals(1);
    m_heightBox->setSingleStep(0.5);
    m_heightBox->setValue(170.0);

    m_weightBox = new QDoubleSpinBox(this);
    m_weightBox->setRange(1.0, 300.0);
    m_weightBox->setDecimals(1);
    m_weightBox->setSingleStep(0.5);
    m_weightBox->setValue(60.0);

    m_mobileEdit = new QLineEdit(this);
    m_mobileEdit->setMaxLength(11);
    m_mobileEdit->setPlaceholderText(QStringLiteral("11 位手机号"));

    m_ageBox = new QSpinBox(this);
    m_ageBox->setRange(0, 150);
    m_ageBox->setValue(36);

    m_createdLabel = new QLabel(this);
    m_createdLabel->setObjectName(QStringLiteral("mutedLabel"));

    form->addRow(QStringLiteral("病历号"), m_idEdit);
    form->addRow(QStringLiteral("身份证号"), m_idCardEdit);
    form->addRow(QStringLiteral("姓名"), m_nameEdit);
    form->addRow(QStringLiteral("性别"), m_sexBox);
    form->addRow(QStringLiteral("出生日期"), m_dobEdit);
    form->addRow(QStringLiteral("身高（cm）"), m_heightBox);
    form->addRow(QStringLiteral("体重（kg）"), m_weightBox);
    form->addRow(QStringLiteral("手机号"), m_mobileEdit);
    form->addRow(QStringLiteral("年龄"), m_ageBox);
    form->addRow(QStringLiteral("建档时间"), m_createdLabel);
    root->addLayout(form);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName(QStringLiteral("formError"));
    m_errorLabel->setWordWrap(true);
    root->addWidget(m_errorLabel);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    auto *saveButton = buttons->addButton(QStringLiteral("保存"), QDialogButtonBox::AcceptRole);
    saveButton->setObjectName(QStringLiteral("primaryButton"));
    saveButton->setDefault(true);
    saveButton->setShortcut(QKeySequence(QStringLiteral("Ctrl+S")));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    root->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::rejected, this, &PatientDialog::reject);
    connect(saveButton, &QPushButton::clicked, this, &PatientDialog::accept);
    connect(m_dobEdit, &QDateEdit::dateChanged, this, &PatientDialog::updateAgeFromDob);

    updateAgeFromDob();
    m_createdLabel->setText(QStringLiteral("保存时自动记录"));
}

void PatientDialog::setRecord(const PatientRecord &record)
{
    m_createdTimestamp = record.createdTimestamp;
    m_idEdit->setText(record.id);
    m_idCardEdit->setText(record.idCard);
    m_nameEdit->setText(record.name);
    m_sexBox->setCurrentIndex(m_sexBox->findData(record.sex));
    if (!record.dob.isEmpty()) {
        const QDate date = QDate::fromString(record.dob, QStringLiteral("yyyy-MM-dd"));
        if (date.isValid())
            m_dobEdit->setDate(date);
    }
    m_heightBox->setValue(record.height);
    m_weightBox->setValue(record.weight);
    m_mobileEdit->setText(record.mobile);
    m_ageBox->setValue(record.age);
    m_createdLabel->setText(record.createdTimestamp);
    m_errorLabel->clear();
}

PatientRecord PatientDialog::record() const
{
    PatientRecord result;
    result.id = m_idEdit->text().trimmed();
    result.idCard = m_idCardEdit->text().trimmed().toUpper();
    result.name = m_nameEdit->text().trimmed();
    result.sex = m_sexBox->currentData().toInt();
    result.dob = m_dobEdit->date().toString(QStringLiteral("yyyy-MM-dd"));
    result.height = m_heightBox->value();
    result.weight = m_weightBox->value();
    result.mobile = m_mobileEdit->text().trimmed();
    result.age = m_ageBox->value();
    result.createdTimestamp = m_createdTimestamp.isEmpty()
        ? QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
        : m_createdTimestamp;
    return result;
}

void PatientDialog::updateAgeFromDob()
{
    const QDate birthday = m_dobEdit->date();
    const QDate today = QDate::currentDate();
    int age = today.year() - birthday.year();
    if (today.month() < birthday.month()
        || (today.month() == birthday.month() && today.day() < birthday.day())) {
        --age;
    }
    m_ageBox->setValue(qBound(0, age, 150));
}

bool PatientDialog::validate(QString *message) const
{
    const QString id = m_idEdit->text().trimmed();
    if (!QRegularExpression(QStringLiteral("^[A-Za-z0-9_-]{3,32}$")).match(id).hasMatch()) {
        *message = QStringLiteral("病历号需为 3～32 位字母、数字或下划线。");
        return false;
    }

    const QString idCard = m_idCardEdit->text().trimmed();
    if (!QRegularExpression(QStringLiteral("^\\d{17}[0-9Xx]$")).match(idCard).hasMatch()) {
        *message = QStringLiteral("身份证号格式不正确，应为 18 位（末位可为 X）。");
        return false;
    }

    const QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        *message = QStringLiteral("姓名不能为空。");
        return false;
    }
    if (name.size() > 20) {
        *message = QStringLiteral("姓名长度不能超过 20 个字符。");
        return false;
    }

    const QString mobile = m_mobileEdit->text().trimmed();
    if (!QRegularExpression(QStringLiteral("^1[3-9]\\d{9}$")).match(mobile).hasMatch()) {
        *message = QStringLiteral("手机号格式不正确，应为 11 位数字。");
        return false;
    }

    if (m_dobEdit->date() > QDate::currentDate()) {
        *message = QStringLiteral("出生日期不能晚于今天。");
        return false;
    }

    bool exists = false;
    QString error;
    const QString excludeId = m_mode == EditMode ? id : QString();
    if (!PatientRepository::idCardExists(idCard.toUpper(), excludeId, &exists, &error)) {
        *message = QStringLiteral("校验身份证号失败：%1").arg(error);
        return false;
    }
    if (exists) {
        *message = QStringLiteral("身份证号已存在，同一患者不能重复建档。");
        return false;
    }
    return true;
}

void PatientDialog::accept()
{
    QString message;
    if (!validate(&message)) {
        m_errorLabel->setText(message);
        return;
    }

    const PatientRecord data = record();
    QString error;
    const bool ok = (m_mode == AddMode)
        ? PatientRepository::insert(data, &error)
        : PatientRepository::update(data, &error);
    if (!ok) {
        m_errorLabel->setText(error);
        return;
    }
    QDialog::accept();
}

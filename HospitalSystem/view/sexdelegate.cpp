#include "sexdelegate.h"

#include <QComboBox>

SexDelegate::SexDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QString SexDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale)
    if (value.isNull())
        return QString();
    return value.toInt() == 1 ? QStringLiteral("男") : QStringLiteral("女");
}

QWidget *SexDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    auto *editor = new QComboBox(parent);
    editor->addItem(QStringLiteral("男"), 1);
    editor->addItem(QStringLiteral("女"), 0);
    return editor;
}

void SexDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto *combo = qobject_cast<QComboBox *>(editor);
    if (!combo)
        return;
    const int current = index.model()->data(index, Qt::EditRole).toInt();
    combo->setCurrentIndex(combo->findData(current));
}

void SexDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                               const QModelIndex &index) const
{
    auto *combo = qobject_cast<QComboBox *>(editor);
    if (!combo)
        return;
    model->setData(index, combo->currentData(), Qt::EditRole);
}

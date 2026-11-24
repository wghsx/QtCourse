#ifndef SEXDELEGATE_H
#define SEXDELEGATE_H

#include <QStyledItemDelegate>

// 自定义代理：把 Patient.SEX 的 0/1 显示为“女/男”，
// 编辑时提供下拉框，避免用户直接输入非法值。
class SexDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SexDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
};

#endif // SEXDELEGATE_H

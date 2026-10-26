#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("关于"));
    connect(ui->btnOk, &QPushButton::clicked, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

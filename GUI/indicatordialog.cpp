#include "indicatordialog.h"
#include "ui_indicatordialog.h"

IndicatorDialog::IndicatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IndicatorDialog)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() |= Qt::FramelessWindowHint);

    ui->buttonBox->setFocus();
    resize(400, 100);
}

IndicatorDialog::~IndicatorDialog()
{
    delete ui;
}

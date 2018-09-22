#include "playdialog.h"
#include "ui_playdialog.h"

PlayDialog::PlayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayDialog)
{
    ui->setupUi(this);
}

PlayDialog::~PlayDialog()
{
    delete ui;
}

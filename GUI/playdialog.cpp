#include "playdialog.h"
#include "ui_playdialog.h"
#include <QFileDialog>

PlayDialog::PlayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayDialog)
{
    ui->setupUi(this);

    //customize settings
    setWindowFlags(this->windowFlags() |= Qt::FramelessWindowHint);
    ui->lineEdit->setPlaceholderText("Enter Ticker or File");
    ui->buttonBox->setFocus();
    resize(400, 100);
    // we don't want to delete on close because we need the input value
    setAttribute( Qt::WA_DeleteOnClose, false);
    connect(ui->pushButton, &QPushButton::pressed, this, &PlayDialog::slotFileLoad);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, &PlayDialog::confirmInput);
}

PlayDialog::~PlayDialog()
{
    delete ui;
}

QString PlayDialog::getInput() const
{
    return userInput;
}

void PlayDialog::slotFileLoad()
{
    auto filePath = QFileDialog::getOpenFileName(this, "Load Tick Data", QString("..\\SampleData\\"), "*.tickdat");
    if(filePath.size() > 0)
    {
        ui->lineEdit->setText(filePath);
    }
}

void PlayDialog::confirmInput()
{
    userInput = ui->lineEdit->text();
}

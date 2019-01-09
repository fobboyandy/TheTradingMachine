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

QStringList PlayDialog::getInput() const
{
    return userInput;
}

bool PlayDialog::getLiveTrading() const
{
    return liveTrading;
}

void PlayDialog::slotFileLoad()
{
    auto filePath = QFileDialog::getOpenFileNames(this, "Load Tick Data", QString("..\\SampleData\\"), "*.tickdat");
    QString fileLoadBoxText;

    for(auto& filePathStr: filePath)
    {
        fileLoadBoxText.push_back(filePathStr);
        fileLoadBoxText.push_back(";");
    }

    // set the gui to display the text of the files chosen.
    ui->lineEdit->setText(fileLoadBoxText);
}

void PlayDialog::confirmInput()
{
    userInput = ui->lineEdit->text().split(';', QString::SplitBehavior::SkipEmptyParts);
    liveTrading = ui->checkBox->checkState() == Qt::CheckState::Checked ? true : false;
}

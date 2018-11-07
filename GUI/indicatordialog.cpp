#include "indicatordialog.h"
#include "ui_indicatordialog.h"

IndicatorDialog::IndicatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IndicatorDialog)
{
    //ui->setupUi(this);
    setWindowFlags(this->windowFlags() |= Qt::FramelessWindowHint);

    ui->buttonBox->setFocus();
    resize(400, 100);


        this->resize(270, 110);
        ui->gridLayout = new QGridLayout(this);
        ui->gridLayout->setObjectName(QStringLiteral("gridLayout"));

        ui->buttonBox = new QDialogButtonBox(this);
        ui->buttonBox->setObjectName(QStringLiteral("buttonBox"));
        ui->buttonBox->setOrientation(Qt::Horizontal);
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        ui->gridLayout->addWidget(ui->buttonBox, 1, 4, 1, 1);


        ui->label = new QLabel(this);
        ui->label->setObjectName(QStringLiteral("label"));

        ui->gridLayout->addWidget(ui->label, 0, 2, 1, 1);


        ui->spinBox = new QSpinBox(this);
        ui->spinBox->setObjectName(QStringLiteral("spinBox"));

        ui->gridLayout->addWidget(ui->spinBox, 0, 3, 1, 1);


        //retranslateUi(this);
        QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

        //QMetaObject::connectSlotsByName(IndicatorDialog);
//    } // setupUi

//    void retranslateUi(QDialog *IndicatorDialog)
//    {
//        IndicatorDialog->setWindowTitle(QApplication::translate("IndicatorDialog", "Dialog", nullptr));
//        label->setText(QApplication::translate("IndicatorDialog", "TextLabel", nullptr));
//    } // retranslateUi



}

IndicatorDialog::~IndicatorDialog()
{
    delete ui;
}

void IndicatorDialog::addSpinbox(QString text, int minVal, int maxVal, int defaultVal)
{

}

void IndicatorDialog::addCheckbox(QString text, bool defaultChecked)
{

}

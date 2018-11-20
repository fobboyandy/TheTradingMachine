#include "indicatordialog.h"

IndicatorDialog::IndicatorDialog(QWidget *parent) :
    QDialog(parent)
{
    gridLayout = new QGridLayout(this);

    //create button box
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    resize(400, 100);
    setWindowFlags(this->windowFlags() |= Qt::FramelessWindowHint);
    setWindowTitle(QString("Indicator Settings"));

    row_ = 0;
    valid_ = false;

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this](){valid_ = true;});
    connect(buttonBox, &QDialogButtonBox::rejected, this, [this](){valid_ = false;});
}

IndicatorDialog::~IndicatorDialog()
{
}


void IndicatorDialog::addSpinbox(QString text, int defaultValue, int minVal, int maxVal)
{
    auto label = new QLabel(this);
    label->setText(text);

    auto spinbox = new QSpinBox(this);
    spinbox->setRange(minVal, maxVal);
    spinbox->setValue(defaultValue);

    gridLayout->addWidget(label, row_, 2, 1, 1);
    gridLayout->addWidget(spinbox, row_, 3, 1, 1);

    ++row_;
    settings_[text.toStdString()] = spinbox;
}

void IndicatorDialog::addCheckbox(QString text, bool defaultChecked)
{

}

int IndicatorDialog::getSpinboxValue(QString text)
{
    if(settings_.find(text.toStdString()) != settings_.end())
    {
        return static_cast<QSpinBox*>(settings_[text.toStdString()])->value();
    }

    return 0;
}

int IndicatorDialog::exec()
{
    //display the buttonbox at the appropriate place
    gridLayout->addWidget(buttonBox, row_, 4, 1, 1);
    buttonBox->setFocus();

    return QDialog::exec();
}

bool IndicatorDialog::valid()
{
    return valid_;
}

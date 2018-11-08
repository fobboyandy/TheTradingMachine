#ifndef INDICATORDIALOG_H
#define INDICATORDIALOG_H

#include <QDialog>
#include <vector>
#include <QSpinBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <unordered_map>
#include <limits>

namespace Ui {
class IndicatorDialog;
}

class IndicatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IndicatorDialog(QWidget *parent = nullptr);
    ~IndicatorDialog() override;

    //inputs as keys are case sensitive
    void addSpinbox(QString text, int defaultValue, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max());
    void addCheckbox(QString text, bool defaultChecked = false);
    int getSpinboxValue(QString text);
    bool getCheckboxValue(QString text);
    int exec() override;
    bool valid();

private:
    std::unordered_map<std::string, QWidget*> settings_;
    QGridLayout *gridLayout;
    int row_;
    bool valid_;

    QDialogButtonBox* buttonBox;
};

#endif // INDICATORDIALOG_H

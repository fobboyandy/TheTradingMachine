#ifndef INDICATORDIALOG_H
#define INDICATORDIALOG_H

#include <QDialog>
#include <vector>
#include <QSpinBox>
#include <QCheckBox>

namespace Ui {
class IndicatorDialog;
}

class IndicatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IndicatorDialog(QWidget *parent = nullptr);
    ~IndicatorDialog();
    void addSpinbox(QString text, int minVal, int maxVal, int defaultVal = 5);
    void addCheckbox(QString text, bool defaultChecked = false);

private:
    Ui::IndicatorDialog *ui;
    std::list<QWidget*> widgets_;

};

#endif // INDICATORDIALOG_H

#ifndef INDICATORDIALOG_H
#define INDICATORDIALOG_H

#include <QDialog>

namespace Ui {
class IndicatorDialog;
}

class IndicatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IndicatorDialog(QWidget *parent = nullptr);
    ~IndicatorDialog();

private:
    Ui::IndicatorDialog *ui;
};

#endif // INDICATORDIALOG_H

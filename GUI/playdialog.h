#ifndef PLAYDIALOG_H
#define PLAYDIALOG_H

#include <QDialog>

namespace Ui {
class PlayDialog;
}

class PlayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayDialog(QWidget *parent = nullptr);
    ~PlayDialog();
    QString getInput() const;
    bool getLiveTrading() const;

private slots:
    void slotFileLoad();
    void confirmInput();

private:
    Ui::PlayDialog *ui;
    QString userInput;
    bool liveTrading;
};

#endif // PLAYDIALOG_H

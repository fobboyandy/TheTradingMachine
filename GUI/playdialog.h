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
    bool getLiveTrading() const;
    QStringList getInput() const;

private slots:
    void slotFileLoad();
    void confirmInput();

private:
    Ui::PlayDialog *ui;
    QStringList userInput;
    bool liveTrading;
};

#endif // PLAYDIALOG_H

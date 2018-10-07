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

private slots:
    void slotFileLoad();

private:
    Ui::PlayDialog *ui;
    QString userInput;
};

#endif // PLAYDIALOG_H

#ifndef COMMANDDIALOG_H
#define COMMANDDIALOG_H

#include <QDialog>

namespace Ui { class CommandDialog; }

class CommandDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommandDialog(QWidget *parent, QString str);
    ~CommandDialog();
    QString name;
    QString command;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CommandDialog *ui;
    QString str;
    QString extra {};
    void on_wrong_input();
};

#endif // COMMANDDIALOG_H

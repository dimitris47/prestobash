#include "commanddialog.h"
#include "ui_commanddialog.h"
#include <QMessageBox>

CommandDialog::CommandDialog(QWidget *parent, QString str)
    : QDialog(parent)
    , ui(new Ui::CommandDialog)
{
    ui->setupUi(this);
    this->str = str;
    if (str == "=")
        extra = " or spaces";
    ui->nameLine->setPlaceholderText("enter shortcut name - must not contain '" + str + "'" + extra);
    ui->commandLine->setPlaceholderText("enter command");
}


CommandDialog::~CommandDialog()
{
    delete ui;
}


void CommandDialog::on_buttonBox_accepted()
{
    if (!ui->nameLine->text().contains(str) && ui->nameLine->text() != "" && ui->commandLine->text() != "") {
        if (str == "=") {
            if (!ui->nameLine->text().contains(" ")) {
                name = ui->nameLine->text();
            } else {
                on_wrong_input();
                return;
            }
        } else {
            name = ui->nameLine->text();
            command = ui->commandLine->text();
        }
        command = ui->commandLine->text();
    } else {
        on_wrong_input();
        return;
    }
}


void CommandDialog::on_wrong_input()
{
    QMessageBox box(QMessageBox::Warning, "Wrong input",
                    "Name must not be empty and must not contain '" + str + "'" + extra +
                    "\nCommand must not be empty",
                    QMessageBox::Ok,
                    this);
    box.exec();
    this->reject();
}

/**
 * Copyright 2021 Dimitris Psathas <dimitrisinbox@gmail.com>
 *
 * This file is part of PrestoBash.
 *
 * PrestoBash is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License  as  published by  the  Free Software
 * Foundation,  either version 3 of the License,  or (at your option)  any later
 * version.
 *
 * PrestoBash is distributed in the hope that it will be useful,  but  WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the  GNU General Public License  for more details.
 *
 * You should have received a copy of the  GNU General Public License along with
 * PrestoBash. If not, see <http://www.gnu.org/licenses/>.
 */


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

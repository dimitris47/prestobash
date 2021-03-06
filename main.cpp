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


#include "global.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("DP Software");
    QApplication::setApplicationName(APPNAME);
    QApplication::setApplicationVersion(VERSION);
    MainWindow w;
    w.show();
    return a.exec();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commanddialog.h"
#include "global.h"

#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextStream>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->aliasBox->setAlignment(Qt::AlignTop);
    ui->appBox->setAlignment(Qt::AlignTop);
    readPreferences();
    ui->statusbar->showMessage(readAliasesAndShortcuts(), 3000);
    ui->currDirLabel->setText("Current dir: " + QDir::currentPath());
    ui->currDirLabel->setToolTip(ui->currDirLabel->text());
    createTrayIcon();
}


MainWindow::~MainWindow()
{
    delete ui;
}


QString MainWindow::bashrcLocation()
{
    return QDir::homePath() + "/.bashrc";
}


QString MainWindow::readAliasesAndShortcuts()
{
    shortcuts = 0;
    QFile bashrc(bashrcLocation());
    QStringList aliasLines;
    if (bashrc.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ts(&bashrc);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            if (line.startsWith("alias ") &&
                    !line.startsWith("alias l=") && !line.startsWith("alias la=") &&
                    !line.startsWith("alias ll=") && !line.startsWith("alias alert=")) {
                aliasLines.append(line);
                const QString name = line.split(" ").at(1).split("=").at(0);
                QString cmd;
                (line.contains("sudo") && !line.contains("sudo -A"))? cmd = line.replace("sudo", "sudo -A") : cmd = line;
                const QString command = cmd.split("='").at(1).split("'").at(0);
                auto newButton = new QPushButton;
                newButton->setText(name);
                newButton->setToolTip(command);
                ui->aliasBox->addWidget(newButton);
                connect(newButton, &QPushButton::clicked, this, [this, name, command]() {
                    doProcess(name, command);
                });
            }
        }
        bashrc.close();
    }

    QString aliasString;
    aliasLines.count() == 1? aliasString = " user alias" : aliasString = " user aliases";

    QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = dataDir.path() + "/prestocommands.txt";
    QFile cmdFile(filePath);
    if (!cmdFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString::number(aliasLines.count()) + aliasString + " found";
    QTextStream cmdData(&cmdFile);
    cmdData.setCodec(QTextCodec::codecForName("UTF-8"));
    QStringList lines;
    while (!cmdData.atEnd()) {
        QString line = cmdData.readLine();
        if (line.contains(">>>"))
            lines.append(line);
    }
    for (auto &&line : lines) {
        const QString shortcutName = line.split(">>>").at(0);
        QString cmd;
        (line.contains("sudo") && !line.contains("sudo -A"))? cmd = line.replace("sudo", "sudo -A") : cmd = line;
        const QString shortcutCommand = cmd.split(">>>").at(1);
        shortcuts++;
        auto newButton = new QPushButton(this);
        newButton->setText(shortcutName);
        newButton->setToolTip(shortcutCommand);
        ui->appBox->addWidget(newButton);
        connect(newButton, &QPushButton::clicked, this, [this, shortcutName, shortcutCommand]() {
            doProcess(shortcutName, shortcutCommand);
        });
    }
    cmdFile.close();

    QString shortcutsString;
    shortcuts == 1? shortcutsString = " in-app shortcut" : shortcutsString = " in-app shortcuts";

    return QString::number(aliasLines.count()) + aliasString + " & " +
            QString::number(shortcuts) + shortcutsString + " found";
}


void MainWindow::on_addAliasButton_clicked()
{
    CommandDialog dialog(this, "=");
    int ret = dialog.exec();
    if (ret == QDialog::Rejected)
        return;

    QFile file(bashrcLocation());
    QStringList aliasLines;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ts(&file);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            if (line.startsWith("alias "))
                aliasLines.append(line);
        }
        file.close();
    }

    const QString name = dialog.name;
    const QString command = dialog.command;
    const QString newLine = "alias " + name + "='" + command + "'\n";
    if (!aliasLines.contains(newLine.split("\n").at(0))) {
        QFile bashrc(bashrcLocation());
        if (!bashrc.open(QIODevice::Append | QFile::Text)) {
            ui->statusbar->showMessage("unable to open file", 3000);
            return;
        }
        QTextStream data(&bashrc);
        data.setCodec(QTextCodec::codecForName("UTF-8"));
        data.setIntegerBase(10);
        data << newLine;
        bashrc.close();
        auto button = new QPushButton;
        button->setText(name);
        button->setToolTip(command);
        ui->aliasBox->addWidget(button);
        connect(button, &QPushButton::clicked, this, [this, name, command]() { doProcess(name, command); });
        ui->statusbar->showMessage("Added: " + newLine, 3000);
    } else {
        ui->statusbar->showMessage("alias already in .bashrc", 3000);
    }
}


void MainWindow::on_addShortcutButton_clicked()
{
    CommandDialog dialog(this, ">>>");
    int ret = dialog.exec();
    if (ret == QDialog::Rejected)
        return;
    const QString name = dialog.name;
    const QString command = dialog.command;
    shortcuts++;

    QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!dataDir.exists())
        dataDir.mkpath(".");
    QString filePath = dataDir.path() + "/prestocommands.txt";

    QFile cmdFile(filePath);
    if (!cmdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->statusbar->showMessage("unable to open file", 3000);
        return;
    }
    QTextStream cmdData(&cmdFile);
    cmdData.setCodec(QTextCodec::codecForName("UTF-8"));
    QStringList lines;
    while (!cmdData.atEnd()) {
        QString line = cmdData.readLine();
        if (line.contains(">>>"))
            lines.append(line);
    }
    cmdFile.close();

    QString newLine = name + ">>>" + command;
    if (!lines.contains(newLine)) {
        QFile file(filePath);
        if (!file.open(QIODevice::Append | QFile::Text)) {
            ui->statusbar->showMessage("unable to open file", 3000);
            return;
        }
        QTextStream data(&file);
        data.setCodec(QTextCodec::codecForName("UTF-8"));
        data.setIntegerBase(10);
        data << newLine + '\n';
        file.close();
        auto newButton = new QPushButton;
        newButton->setText(name);
        newButton->setToolTip(command);
        ui->appBox->addWidget(newButton);
        connect(newButton, &QPushButton::clicked, this, [this, name, command]() { doProcess(name, command); });
        ui->statusbar->showMessage("Added: " + newLine, 3000);
    } else {
        ui->statusbar->showMessage("shortcut already exists", 3000);
    }
}


void MainWindow::doProcess(QString name, QString command)
{
    ui->textBrowser->clear();
    ui->statusbar->showMessage(name + "started");

    cmd = command;
    QRegularExpression re (" *[&&|;] *");

    while (cmd.contains(re)) {
        QString run = cmd.split(re).at(0);

        QProcess process;
        connect(&process, &QProcess::readyReadStandardOutput, this, [this, &process]() {
            ui->textBrowser->QTextEdit::append(process.readAllStandardOutput());
        });
        connect(&process, &QProcess::readyReadStandardError, this, [this, &process]() {
            ui->textBrowser->QTextEdit::append(process.readAllStandardError());
        });
        process.start(run);

        if (process.waitForFinished(-1)) {
            ui->statusbar->showMessage(run + " finished", 3000);

            cmd = cmd.split(run).at(1).section(re, 0, -1, QString::SectionSkipEmpty);

            if (!cmd.contains(re)) {
                QProcess process;
                connect(&process, &QProcess::readyReadStandardOutput, this, [this, &process]() {
                    ui->textBrowser->QTextEdit::append(process.readAllStandardOutput());
                });
                connect(&process, &QProcess::readyReadStandardError, this, [this, &process]() {
                    ui->textBrowser->QTextEdit::append(process.readAllStandardError());
                });
                process.start(cmd);
                process.waitForFinished(-1);
                ui->statusbar->showMessage(cmd + " executed successfully");
                return;
            }
        } else {
            QMessageBox::warning(this, "Execution failed", "Execution of " + cmd + " failed.");
            return;
        }
    }

    QProcess process;
    connect(&process, &QProcess::readyReadStandardOutput, this, [this, &process]() {
        ui->textBrowser->QTextEdit::append(process.readAllStandardOutput());
    });
    connect(&process, &QProcess::readyReadStandardError, this, [this, &process]() {
        ui->textBrowser->QTextEdit::append(process.readAllStandardError());
    });
    process.start(command);
    process.waitForFinished(-1);
    ui->statusbar->showMessage(name + " executed successfully");
}


void MainWindow::on_editBashrcButton_clicked()
{
    QFile bashrc(bashrcLocation());
    QStringList aliasLines;
    if (bashrc.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ts(&bashrc);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        const QString fileContent = ts.readAll();
        bashrc.close();

        QDialog dialog(this);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        dialog.setLayout(layout);
        QLabel *label = new QLabel("Caution: changes to this file may affect the functionality of your system", &dialog);
        layout->addWidget(label);
        QTextEdit *te = new QTextEdit(&dialog);
        layout->addWidget(te);
        te->setText(fileContent);
        QDialogButtonBox *box = new QDialogButtonBox(&dialog);
        box->addButton(QDialogButtonBox::Cancel);
        box->addButton(QDialogButtonBox::Save);
        layout->addWidget(box);
        connect(box, SIGNAL(accepted()), &dialog, SLOT(accept()));
        connect(box, SIGNAL(rejected()), &dialog, SLOT(reject()));
        int ret = dialog.exec();
        if (ret == QDialog::Rejected) {
            ui->statusbar->showMessage("changes not saved", 3000);
            return;
        }
        QFile file(bashrcLocation());
        if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
            ui->statusbar->showMessage("unable to save to file", 3000);
            return;
        }
        QTextStream data(&file);
        data.setCodec(QTextCodec::codecForName("UTF-8"));
        data.setIntegerBase(10);
        data << te->toPlainText() + '\n';
        file.close();
        ui->statusbar->showMessage("file saved", 3000);

        on_actionRefresh_triggered();
    }
}


void MainWindow::on_editShortcutsButton_clicked()
{
    QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!dataDir.exists())
        dataDir.mkpath(".");
    QString filePath = dataDir.path() + "/prestocommands.txt";
    QFile cmdFile(filePath);
    if (!cmdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->statusbar->showMessage("unable to open file", 3000);
        return;
    }
    QTextStream cmdData(&cmdFile);
    cmdData.setCodec(QTextCodec::codecForName("UTF-8"));
    const QString fileContent = cmdData.readAll();
    cmdFile.close();

    QDialog dialog(this);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    dialog.setLayout(layout);
    QTextEdit *te = new QTextEdit(&dialog);
    layout->addWidget(te);
    te->setText(fileContent);
    QDialogButtonBox *box = new QDialogButtonBox(&dialog);
    box->addButton(QDialogButtonBox::Cancel);
    box->addButton(QDialogButtonBox::Save);
    layout->addWidget(box);
    connect(box, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(box, SIGNAL(rejected()), &dialog, SLOT(reject()));
    int ret = dialog.exec();
    if (ret == QDialog::Rejected) {
        ui->statusbar->showMessage("changes not saved", 3000);
        return;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        ui->statusbar->showMessage("unable to save to file", 3000);
        return;
    }
    QTextStream data(&file);
    data.setCodec(QTextCodec::codecForName("UTF-8"));
    data.setIntegerBase(10);
    data << te->toPlainText() + '\n';
    file.close();
    ui->statusbar->showMessage("file saved", 3000);

    on_actionRefresh_triggered();
}


void MainWindow::on_actionSetCurrentDir_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Choose Directory",
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!QDir::setCurrent(dir)) {
        QMessageBox::warning(this, "Directory change failure", "Could not change to selected directory");
    } else {
        ui->currDirLabel->setText("Current dir: " + dir);
        ui->currDirLabel->setToolTip(ui->currDirLabel->text());
        ui->statusbar->showMessage("directory set to " + dir, 3000);
    }
}


void MainWindow::on_actionRefresh_triggered()
{
    for (auto &&button : findChildren<QPushButton *>())
        if (button != ui->addAliasButton && button != ui->editBashrcButton
            && button != ui->addShortcutButton && button != ui->editShortcutsButton)
        button->close();
    ui->gridLayout->invalidate();
    readAliasesAndShortcuts();
}


void MainWindow::on_actionAppInfo_triggered()
{
    QMessageBox::about(this, "Program Info",
                       (QApplication::applicationName() + " " + QApplication::applicationVersion() +
                        br.repeated(2) + APPINFO));
}


void MainWindow::createTrayIcon()
{
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/icons/prestobash.png"), this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(on_toggleShow()));
    QAction *show_hide_action = new QAction("Show/Hide", trayIcon);
    connect(show_hide_action, SIGNAL(triggered()), this, SLOT(on_toggleShow()));
    QAction *quit_action = new QAction("Exit", trayIcon);
    connect(quit_action, SIGNAL(triggered()), this, SLOT(on_exit()));

    trayIconMenu = new QMenu;
    trayIconMenu->addAction(show_hide_action);
    trayIconMenu->addAction(quit_action);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}


void MainWindow::on_toggleShow()
{
    QSettings settings;
    if (isVisible()) {
        settings.setValue("geometry", saveGeometry());
        hide();
    } else {
        show();
        const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
        restoreGeometry(geometry);
        raise();
        setFocus();
    }
    settings.sync();
}


void MainWindow::on_exit()
{
    this->close();
}


void MainWindow::readPreferences()
{
    QSettings settings;
    bool isMax = settings.value("isMaximized", false).toBool();
    if (isMax) {
        showMaximized();
    } else {
        const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
        restoreGeometry(geometry);
    }
}


void MainWindow::savePreferences()
{
    QSettings settings;
    settings.setValue("isMaximized", isMaximized());
    if (!isMaximized())
        settings.setValue("geometry", saveGeometry());
    settings.sync();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    savePreferences();
    event->accept();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


protected:
    void savePreferences();
    void closeEvent(QCloseEvent *event) override;


private slots:
    void on_actionAppInfo_triggered();
    void on_actionRefresh_triggered();
    void on_actionSetCurrentDir_triggered();
    void on_addAliasButton_clicked();
    void on_addShortcutButton_clicked();
    void on_editBashrcButton_clicked();
    void on_editShortcutsButton_clicked();
    void on_toggleShow();
    void on_exit();


private:
    Ui::MainWindow *ui;
    void createTrayIcon();
    void doProcess(QString name, QString command);
    void readPreferences();
    QString readAliasesAndShortcuts();
    QString bashrcLocation();
    QString cmd;
    int shortcuts;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // MAINWINDOW_H

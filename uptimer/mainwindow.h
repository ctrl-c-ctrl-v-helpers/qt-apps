#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QSystemTrayIcon>
#include "trayiconimage.h"


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

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    void timerTimeout();
    void timeStep(bool forceUpdate );
    QDateTime start;

    QSystemTrayIcon *trayIcon;
    void clicked();
    int oldSeconds;
    TrayIconImage image;

    static inline const int TimeToAutoHideSeconds{5};

};


#endif // MAINWINDOW_H

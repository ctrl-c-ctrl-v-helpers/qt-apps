#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
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
    QImage image;

    static inline const QColor gold{ 255, 215, 0};
    static inline const QColor black{ 0, 0, 0};
    static inline const QColor green{ 10, 230, 168};
    static inline const QColor red{ 255, 0, 0};
};
#endif // MAINWINDOW_H

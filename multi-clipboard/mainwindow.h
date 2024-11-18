#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

public slots:
    void getClicked();
    void setClicked();
    void clipboardChanged();
private:
    const int numStore = 10;
    const int numHist = 10;
    const QString getText = "Store ";
    const QString setText = "Copy ";
    Ui::MainWindow *ui;
    void createLayout();

};
#endif // MAINWINDOW_H

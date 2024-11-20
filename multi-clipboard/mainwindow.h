#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;

}
class QPushButton;
class QLabel;
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
    std::vector<QPushButton*> getButtons;
    std::vector<QPushButton*> setButtons;
    std::vector<QLabel*> labels;
    Ui::MainWindow *ui;
    void createLayout();
    virtual void changeEvent(QEvent * event);

    void setColorTheme();
};
#endif // MAINWINDOW_H

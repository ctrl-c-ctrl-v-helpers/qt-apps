#ifndef CLEARFORMATAPP_H
#define CLEARFORMATAPP_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class ClearFormatApp;
}
QT_END_NAMESPACE

class ClearFormatApp : public QMainWindow
{
    Q_OBJECT

public:
    ClearFormatApp(QWidget *parent = nullptr);
    ~ClearFormatApp();

public slots:
    void doClose();


private:
    Ui::ClearFormatApp *ui;
    QTimer timer;

    void doClearFormat();

};
#endif // CLEARFORMATAPP_H

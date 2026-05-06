#ifndef MENUSTOP_H
#define MENUSTOP_H

#include <QMainWindow>
#include "lnk.h"
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MenuStop;
}
QT_END_NAMESPACE

class MenuStop : public QMainWindow
{
    Q_OBJECT

public:
    MenuStop(QWidget *parent = nullptr);
    ~MenuStop();
    void readConfig();
    void checkFilesForShortcuts(const QString &path);
    void populateGrid();

protected:
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MenuStop *ui;
    QString dirPath;
    QPoint windowPos;
    QVector<Lnk> shortcuts;
    int iconSize;
};
#endif // MENUSTOP_H

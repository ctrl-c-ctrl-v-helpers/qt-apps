/* This file was created with AI assistance */

#ifndef MENUSTOP_H
#define MENUSTOP_H

#include <QMainWindow>
#include "lnk.h"
#include <QVector>
#include "subdirwindow.h"

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
    void checkFilesForShortcuts(const QString &path, QVector<Lnk> & shortcuts);
    void runIconsThreads( QVector<Lnk> & shortcuts );
    void populateGrid();
    int getXPos( int x );

protected:
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void showVersionDialog();

private:
    Ui::MenuStop *ui;
    QString dirPath;
    QString iconPath;
    QString configPath;
    QString menuName;
    int windowOffsetY;
    QVector<Lnk> shortcuts;
    int iconSize;
    QVector<int> xPositions;
    QString menuColorStart;
    QString menuColorStop;

    SubDirWindow * subDirWindow;
    int subDirId;

    QGridLayout *gridLayout;
};
#endif // MENUSTOP_H

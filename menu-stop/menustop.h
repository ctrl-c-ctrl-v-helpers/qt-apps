/* This file was created with AI assistance */

#ifndef MENUSTOP_H
#define MENUSTOP_H

#include <QMainWindow>
#include "lnk.h"
#include <QVector>
#include "subdirwindow.h"
#include "config.h"
#include <windows.h>
#include "hoverbutton.h"

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
    void checkFilesForShortcuts(const QString &path, QVector<Lnk> & shortcuts);
    void populateGrid();
    int getXPos( int x );

    void requestCloseChild();

    void minimizeApp();

protected:
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void showVersionDialog();

public:
    Ui::MenuStop *ui;

    QVector<Lnk> shortcuts;

    QVector<int> xPositions;


    SubDirWindow * subDirWindow;
    int subDirId;

    QGridLayout *gridLayout;

    Config *config;

    int iconThreadsNum;

    const int ID_CTRL_SPACE = 1002;

    int hoveredButtonId;

    bool ctrlSpaceRegistered;

    int buttonsColumnId;

    bool reversedExpansion;

    bool activatedByHotkey;

    HoverButton * buttonAtPosition();

};
#endif // MENUSTOP_H

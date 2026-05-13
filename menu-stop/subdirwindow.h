#ifndef MYFRAMELESSWINDOW_H
#define MYFRAMELESSWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include "lnk.h"

class SubDirWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SubDirWindow(QVector<Lnk> & shortc, int iconSz, QPoint leftBottom, QWidget *parent = nullptr);
    void populateGrid();
    ~SubDirWindow();
    void closeUpwards();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUi();
    SubDirWindow * subDirWindow;
    QVector<Lnk> & shortcuts;
    int iconSize;
    QPoint leftBottomCorner;
    int subDirId;
};

#endif // MYFRAMELESSWINDOW_H

/* This file was created with AI assistance */

#ifndef MYFRAMELESSWINDOW_H
#define MYFRAMELESSWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include "lnk.h"
#include "config.h"
#include "hoverbutton.h"

class SubDirWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SubDirWindow(QVector<Lnk> & shortc, Config * configuration, QPoint leftBottom, bool reversedExpand, QWidget *parent = nullptr);
    void populateGrid();
    ~SubDirWindow();
    void closeUpwards();

    void requestCloseChild();

protected:
    void keyPressEvent(QKeyEvent *event) override;

public:
    void setupUi();
    SubDirWindow * subDirWindow;
    QVector<Lnk> & shortcuts;
    Config *config;
    QPoint leftBottomCorner;
    int subDirId;
    int hoveredButtonId;
    int buttonsColumnId;
    QGridLayout *gridLayout;
    bool reversedExpansion;

    HoverButton * buttonAtPosition();
};

#endif // MYFRAMELESSWINDOW_H

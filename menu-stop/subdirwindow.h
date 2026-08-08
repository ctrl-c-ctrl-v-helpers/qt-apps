/* This file was created with AI assistance */

#ifndef MYFRAMELESSWINDOW_H
#define MYFRAMELESSWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include "lnk.h"
#include "config.h"

class SubDirWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SubDirWindow(QVector<Lnk> & shortc, Config * configuration, QPoint leftBottom, QWidget *parent = nullptr);
    void populateGrid();
    ~SubDirWindow();
    void closeUpwards();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUi();
    SubDirWindow * subDirWindow;
    QVector<Lnk> & shortcuts;
    Config *config;
    QPoint leftBottomCorner;
    int subDirId;
};

#endif // MYFRAMELESSWINDOW_H

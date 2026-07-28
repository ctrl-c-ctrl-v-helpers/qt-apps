/* This file was created with AI assistance */

#ifndef VERTICALLABEL_H
#define VERTICALLABEL_H

#include <QLabel>

class VerticalLabel : public QLabel
{
    Q_OBJECT

public:
    VerticalLabel(QWidget *parent = nullptr);

signals:
    void versionClicked(); // Custom signal to notify your main window

protected:
    void paintEvent(QPaintEvent *) override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    int maxClickableY;
};


#endif // VERTICALLABEL_H

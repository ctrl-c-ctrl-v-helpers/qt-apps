#ifndef VERTICALLABEL_H
#define VERTICALLABEL_H

#include <QLabel>

class VerticalLabel : public QLabel
{
public:

protected:
    void paintEvent(QPaintEvent *) override;
    QSize sizeHint() const override;
};


#endif // VERTICALLABEL_H

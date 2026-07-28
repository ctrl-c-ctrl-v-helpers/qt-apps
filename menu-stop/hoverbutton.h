/* This file was created with AI assistance */

#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QPushButton>
#include <QEnterEvent>
#include <QEvent>

class HoverButton : public QPushButton
{
    Q_OBJECT

public:
    explicit HoverButton(QWidget *parent = nullptr);
    explicit HoverButton(const QString &text, QWidget *parent = nullptr);

signals:
    // Sygnał, który wyemitujemy, gdy mysz najedzie na przycisk
    void mouseEntered();
    void mouseLeft();

protected:
    // Nadpisujemy zdarzenia wejścia i wyjścia myszy
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // HOVERBUTTON_H
